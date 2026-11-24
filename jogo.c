#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jogo.h"
#include "timer.h"

// conta quantos pedidos tem na AVL
static int contaPedidos(NodoAvl *raiz){
    if(raiz == NULL) 
        return 0;
    
    return 1 + contaPedidos(raiz->esq) + contaPedidos(raiz->dir);
}

// preenche um vetor com os códigos em ordem crescente
static void preencheCodigos(NodoAvl *raiz, int *vetor, int *posicao){
    if(raiz == NULL) 
        return;
    
    preencheCodigos(raiz->esq, vetor, posicao);

    vetor[*posicao] = raiz->dado.cod;
    (*posicao)++;
    preencheCodigos(raiz->dir, vetor, posicao);
}

// sorteia um índice do vetor de códigos entre 0 e limite-1
static int sorteiaIndice(int limite) {
    long long numero = rand(); // gera um número aleatório de 0 a RAND_MAX

    numero = numero * (RAND_MAX + 1LL) + rand(); // combina dois números aleatórios para aumentar a faixa

    return (int)(numero % limite); // retorna um índice entre 0 e limite-1
}

// identifica os ingredientes que precisam ser preparados e armazenados no estoque
// retorna 1 se o ingrediente pertence ao estoque e 0 caso contrário
static int identificaIngrediente(char simbolo, IngredienteEstoque *ing){
    switch(simbolo){
        case 's':
            *ing = QUEIJO_SUICO;
            break;
        case 'q':
            *ing = QUEIJO_CHEDDAR;
            break;
        case 'c':
            *ing = CEBOLA;
            break;
        case 't':
            *ing = TOMATE;
            break;
        case 'n':
            *ing = CARNE;
            break;
        case 'o':
            *ing = OVO;
            break;
        case 'b':
            *ing = BACON;
            break;
        default:
            return 0;
    }

    return 1;
}

// Verifica se o símbolo corresponde a algum ingrediente válido do jogo.
static int simboloValido(char simbolo) {
    switch (simbolo) {
        case 'z': // pão superior
        case 'k': // ketchup
        case 'm': // mostarda
        case 'p': // picles
        case 'a': // alface
        case 's': // queijo suíço
        case 'q': // queijo cheddar
        case 'c': // cebola
        case 't': // tomate
        case 'n': // carne
        case 'o': // ovo
        case 'b': // bacon
            return 1; // símbolo reconhecido

        default:
            return 0; // símbolo inválido
    }
}

// consulta primeiro hash e, em caso de falta, busca na AVL
// retorna 1 se encontrou o pedido, 0 se não encontrou, e -1 em caso de erro
static int consultaPedido(Jogo *j, int cod, Pedido *pedido){
    if (hashBuscar(&j->cache, cod, pedido)) {
        return 1; // encontrado no hash  
    }
    
    // registra as métricas antes da busca para calcular o custo desta operação
    Contador antes = avlObterMetricas();

    Timer t;
    timer_iniciar(&t);

    NodoAvl *nodo = avlBuscar(j->raizPedidos, cod);

    double tempo = timer_parar(&t);

    Contador depois = avlObterMetricas();

    // acumula as métricas das buscas realizadas na AVL durante a partida
    j->buscas.buscas++;
    j->buscas.comparacoes += depois.comparacoes - antes.comparacoes;
    j->buscas.tempo += tempo;

    if (nodo == NULL)
        return 0; // não encontrado na AVL
    
    *pedido = nodo->dado; // copia o pedido encontrado

    if (!hashInserir(&j->cache, *pedido)) {
        return -1; // erro de memória ao inserir no hash
    }

    return 1; // encontrado na AVL e inserido no hash
}

// avança a partida após uma ação que consome turno ser bem sucedida
static void avancaTurno(Jogo *j){ 
    j->turno++;

    verificaVencimentos(&j->estoque, j->turno); // elimina itens vencidos no comeco desse turno

    j->paciencia--; 

    if (j->paciencia <= 0){ // cliente desiste -> derrota
        j->status = JOGO_DERROTA;
        return;
    }

}

// libera as estruturas utilizadas na partida, preservando a AVL de pedidos
void destroiJogo(Jogo *j) {
    if (j == NULL) 
        return;

    esvaziaFila(&j->clientes);
    esvaziaPilha(&j->prato);
    destroiLista(&j->receita);
    hashDestruir(&j->cache);

    memset(j, 0, sizeof(Jogo)); // limpa a estrutura do jogo
}

// inicializa as estruturas do jogo, sorteia os pedidos e monta a fila de clientes
// retorna 1 em caso de sucesso e 0 em caso de falha
int iniciaJogo(Jogo *j, NodoAvl *raiz, int numClientes, int pacienciaMax) {
    if (j == NULL || raiz == NULL || numClientes <= 0 || numClientes > MAX_CLIENTES || pacienciaMax <= 0 || pacienciaMax > MAX_PACIENCIA)
        return 0; // parâmetros inválidos
    
    memset(j, 0, sizeof(Jogo)); // inicializa a estrutura do jogo

    j->raizPedidos = raiz;
    j->turno = 1;
    j->paciencia = pacienciaMax;
    j->pacienciaMax = pacienciaMax;
    j->atendidos = 0;
    j->totalClientes = numClientes;
    j->pontuacao = 0; 
    j->status = JOGO_ANDAMENTO;

    inicializaEstoque(&j->estoque);
    inicializaPilha(&j->prato);
    inicializaFila(&j->clientes);
    inicializaLista(&j->receita);

    if (!hashInicializar(&j->cache, HASH_M_PADRAO)) {
        destroiJogo(j);
        return 0; // falha ao inicializar o hash 
    }

    // descobre quantos pedidos tem na AVL
    int totalPedidos = contaPedidos(raiz);

    if (totalPedidos == 0) {
        destroiJogo(j);
        return 0; // não há pedidos
    }

    // cria um vetor temporario com os codigos existentes
    int *codigos = (int *)malloc(totalPedidos * sizeof(int));

    if (codigos == NULL) {
        destroiJogo(j);
        return 0; // falha de memória
    }

    int posicao = 0;
    preencheCodigos(raiz, codigos, &posicao);

    // gera os clientes e seus respectivos pedidos
    for (int i = 0; i < numClientes; i++) {
        Cliente cliente;

        snprintf(cliente.nome, sizeof(cliente.nome), "Cliente %d", i + 1);

        int indice = sorteiaIndice(totalPedidos);
        int cod = codigos[indice];

        int resultado = consultaPedido(j, cod, &cliente.pedido);

        if (resultado != 1){
            free(codigos);
            destroiJogo(j);
            return 0; // falha ao consultar o pedido
        }
         
        if (enqueue(&j->clientes, cliente) != 1) {
            free(codigos);
            destroiJogo(j);
            return 0; // falha ao enfileirar o cliente
        }
    }
    free(codigos);

    // prepara a receita do primeiro cliente
    Cliente clienteAtual;

    if(frenteFila(&j->clientes, &clienteAtual) != 1 || !montaReceita(&j->receita, clienteAtual.pedido.ingredientes)) {
        destroiJogo(j);
        return 0; // falha ao obter o cliente atual
    }

    // verifica vencimentos no inicio do primeiro turno
    verificaVencimentos(&j->estoque, j->turno);

    return 1; // sucesso ao inicializar o jogo
}

// prepara um ingrediente e armazena no estoque
ResultadoAcao jogoPrepara(Jogo *j, char simbolo){
    IngredienteEstoque ing;
    int resultado;

    if (j == NULL || j->status != JOGO_ANDAMENTO){
        return ACAO_JOGO_ENCERRADO;
    }

    if (!identificaIngrediente(simbolo, &ing)){ // chama a função e confere se o retorno é válido
        return ACAO_SIMBOLO_INVALIDO;
    }

    resultado = preparaIngrediente(&j->estoque, ing, j->turno);
    
    if (resultado == 0){ // estoque cheio
        return ACAO_ESTOQUE_CHEIO;
    }

    if (resultado < 0){ // erro em preparaIngrediente
        return ACAO_ERRO_INTERNO;
    }

    avancaTurno(j); // tudo certo -> avança turno

    return ACAO_SUCESSO;
}

// adiciona um ingrediente ao hambúrguer, consumindo o estoque quando necessário
// ao adicionar o pão superior, verifica se o pedido foi concluído corretamente
ResultadoAcao jogoAdiciona(Jogo *j, char simbolo){
    char topo;
    IngredienteEstoque ing;
    int resultado; // usado para verificar os retornos das funcoes
    int precisaEstoque;

    // impede alterações se a partida nao estiver em andamento
    if (j == NULL || j->status != JOGO_ANDAMENTO){
        return ACAO_JOGO_ENCERRADO;
    }

    if (!simboloValido(simbolo))
        return ACAO_SIMBOLO_INVALIDO;

    resultado = topoPilha(&j->prato, &topo); // consulta o ingrediente que esta no topo da pilha

    if (resultado == -1)
        return ACAO_ERRO_INTERNO;

    if (resultado == 1 && topo == 'z') // procura pao no topo. Se sim, prato fechado
        return ACAO_PRATO_FECHADO;
    
    // verifica se o ingrediente precisa ser retirado do estoque
    precisaEstoque = identificaIngrediente(simbolo, &ing); // transforma simbolo em ingrediente

    if (precisaEstoque){
        resultado = consultaEstoque(&j->estoque, ing); 
    
        if (resultado < 0)
            return ACAO_ERRO_INTERNO;

        if (resultado == 0)
            return ACAO_SEM_ESTOQUE;
    }

    // adiciona o ingrediente ao topo da pilha
    resultado = push(&j->prato, simbolo);

    if (resultado == 0)
        return ACAO_ERRO_MEMORIA;

    if (resultado < 0)
        return ACAO_ERRO_INTERNO;

    if (precisaEstoque){
        resultado = consomeIngrediente(&j->estoque, ing);

        if (resultado != 1){
            pop(&j->prato, &topo); // se falhar, desfaz a adição na pilha

            if (resultado == 0)
                return ACAO_SEM_ESTOQUE;

            return ACAO_ERRO_INTERNO;  
        }
    }



    if (simbolo == 'z') {

        // Confere se o hamburguer montado corresponde a receita.
        resultado = pilhaConfereReceita(&j->prato, &j->receita);

        if (resultado < 0)
            return ACAO_ERRO_INTERNO;

        if (resultado == 1) {

            Cliente atendido;

            // Se este for o ultimo cliente, nao precisamos
            // preparar uma receita para um proximo cliente.
            if (j->atendidos + 1 == j->totalClientes) {

                // Remove o cliente atendido da fila.
                if (dequeue(&j->clientes, &atendido) != 1) {
                    pop(&j->prato, &topo);
                    return ACAO_ERRO_INTERNO;
                }

                // Registra o atendimento do ultimo cliente.
                j->atendidos++;

                // Avanca o turno, pois adicionar o pao superior consome uma acao.
                j->turno++;

                // Descarta os ingredientes que venceram no novo turno.
                // Isso mantem o estoque atualizado mesmo no encerramento da partida.
                verificaVencimentos(&j->estoque, j->turno);

                // Atualiza a pontuacao (numero de clientes atendidos).
                j->pontuacao++;

                // Como todos os clientes foram atendidos, encerra com vitoria.
                j->status = JOGO_VITORIA;

                // nao ha proximo cliente: encerra a acao aqui
                return ACAO_SUCESSO;
            }

            /* ainda existem clientes depois do atual.
            primeiro prepara a receita do proximo cliente em uma lista temporaria
            para evitar inconsistencias na partida em caso de falha de memoria.
            */
            Cliente proximo;
            ListaEnc proximaReceita;

            inicializaLista(&proximaReceita);

            // Consulta o segundo cliente sem alterar a fila.
            if (segundoFila(&j->clientes, &proximo) != 1) {
                destroiLista(&proximaReceita);
                pop(&j->prato, &topo);
                return ACAO_ERRO_INTERNO;
            }

            // Monta a receita do proximo cliente.
            if (!montaReceita(&proximaReceita, proximo.pedido.ingredientes)) {

                destroiLista(&proximaReceita);

                // Desfaz a insercao do pao superior.
                pop(&j->prato, &topo);

                return ACAO_ERRO_MEMORIA;
            }

            /*
             * A proxima receita ja foi preparada.
             * Agora podemos remover o cliente atual.
             */
            if (dequeue(&j->clientes, &atendido) != 1) {

                destroiLista(&proximaReceita);
                pop(&j->prato, &topo);

                return ACAO_ERRO_INTERNO;
            }

            // Registra o atendimento.
            j->atendidos++;
            j->turno++;

            // Atualiza a pontuacao (numero de clientes atendidos).
            j->pontuacao++;

            // Esvazia o hamburguer do cliente anterior.
            esvaziaPilha(&j->prato);

            // Libera a receita anterior.
            destroiLista(&j->receita);

            // Transfere a nova receita para a estrutura principal.
            j->receita = proximaReceita;

            // Reinicia a paciencia para o proximo cliente.
            j->paciencia = j->pacienciaMax;

            // Elimina ingredientes que venceram no novo turno.
            verificaVencimentos(&j->estoque, j->turno);

            return ACAO_SUCESSO;
        }
    }

    avancaTurno(j); // se o pedido nao foi concluido, avanca turno normalmente

    return ACAO_SUCESSO;
}

// remove o ingrediente do topo da pilha sem devolvê-lo ao estoque
// a remoção consome um turno
ResultadoAcao jogoRemove(Jogo *j){
    char removido;
    int resultado; // usado para verificar o retorno das funções

    if (j == NULL || j->status != JOGO_ANDAMENTO)
        return ACAO_JOGO_ENCERRADO;

    resultado = pop(&j->prato, &removido); // remove o ingrediente do topo da pilha

    if (resultado == 0) // prato vazio
        return ACAO_PRATO_VAZIO;
    
    if (resultado == -1) // erro na funcao pop
        return ACAO_ERRO_INTERNO;

    avancaTurno(j); // remoção consome turno

    return ACAO_SUCESSO;
}

// encerra a partida por abandono sem consumir um turno
ResultadoAcao jogoAbandona(Jogo *j){
    // nao faz nada se o ponteiro for inválido ou a partida já terminou.
    if (j == NULL || j->status != JOGO_ANDAMENTO) {
        return ACAO_JOGO_ENCERRADO;
    }

    // registra que a partida foi abandonada
    j->status = JOGO_ABANDONO;

    return ACAO_SUCESSO;
}