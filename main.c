#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>

#include "timer.h"
#include "avl_tad.h"
#include "carregamento.h"
#include "historico.h"
#include "metricas.h"
#include "hash_tad.h"
#include "busca.h"
#include "lista_contig_tad.h"
#include "fila_tad.h"
#include "ordenacao.h"
#include "jogo.h"

/* PRA QUANDO EXISTIR A LOGICA DO JOGO:
int buscarPedido(NodoAvl *raiz, TabelaHash *cache, int cod, Pedido *out, MetricasBusca *mb);
int salvarPartida(const char *arq, ListaContig *lista, const char *nome, int pontuacao);
int mostrarHistorico(const ListaContig *lista, int porPontuacao, double *tempo);
*/

// le um numero inteiro, rejeitando entradas invalidas
// retorna 1 se valido, 0 se invalido e -1 se chegar ao EOF
int leInteiro(int *valor) {
    char linha[100];
    char extra;

    // tenta ler uma linha da entrada
    if (fgets(linha, sizeof(linha), stdin) == NULL)
        return -1;

    // verifica se a linha foi lida por completo
    if (strchr(linha, '\n') == NULL) {

        // se a linha for maior que o vetor, descarta o restante
        if (!feof(stdin)) {
            int c;

            while ((c = getchar()) != '\n' && c != EOF) {}

            // rejeita a linha longa
            return 0;
        }
    }

    // verifica se existe exatamente um numero inteiro na linha
    if (sscanf(linha, "%d %c", valor, &extra) != 1)
        return 0;

    return 1;
}

// le o nome do jogador
// retorna 1 em caso de sucesso e 0 em caso de erro
static int leNomeJogador(char *nome, int tamanho) {
    printf("Digite seu nome: ");

    if (fgets(nome, tamanho, stdin) == NULL) // le o nome respeitando o tamanho do vetor
        return 0;

    if (strchr(nome, '\n') == NULL && !feof(stdin)) { // verifica se a linha foi lida completamente
        int c;

        // descarta os caracteres restantes
        while ((c = getchar()) != '\n' && c != EOF) {}

        printf("Nome muito longo.\n");
        return 0;
    }

    nome[strcspn(nome, "\r\n")] = '\0'; // remove a quebra de linha

    if (nome[0] == '\0')  // impede nomes vazios
        return 0;

    return 1;
}

// le um unico simbolo de ingrediente
// retorna 1 em caso de sucesso, 0 se der erro de leitura, -1 se for entrada inválida
static int leSimbolo(char *simbolo){
    char linha[64]; 
    char extra;

    printf("Digite o simbolo: ");

    if (fgets(linha, sizeof(linha), stdin) == NULL) // le o simbolo e verifica o retorno
        return 0;

    if (sscanf(linha, " %c %c", simbolo, &extra) != 1) // verifica se existe outro caracter
        return -1;

    return 1;
}

// exibe quantidade disponivel de cada ingrediente
static void exibeEstoque(const Estoque *estoque){
    printf("--=--=--=--ESTOQUE--=--=--\n");
    printf("Carne (n):          %d\n", consultaEstoque(estoque, CARNE));
    printf("Ovo frito (o):      %d\n", consultaEstoque(estoque, OVO));
    printf("Bacon (b):          %d\n", consultaEstoque(estoque, BACON));
    printf("Queijo suico (s):   %d\n", consultaEstoque(estoque, QUEIJO_SUICO));
    printf("Queijo cheddar (q): %d\n", consultaEstoque(estoque, QUEIJO_CHEDDAR));
    printf("Cebola (c):         %d\n", consultaEstoque(estoque, CEBOLA));
    printf("Tomate (t):         %d\n", consultaEstoque(estoque, TOMATE));
    printf("--=--=--=--=--=--=--=--=--\n\n");
}

// imprime os ingredientes do pedido
static void exibePedido(const Jogo *j){
    Cliente atual;

    // obtem os dados do primeiro cliente da fila
    if (frenteFila(&j->clientes, &atual) != 1) { // verifica possivel erro na funcao
        printf("Nao foi possivel consultar o pedido atual.\n");
        return;
    }
    
    printf("\n--=--=--=-- PEDIDO ATUAL --=--=--=--\n");
    printf("Cliente: %s\n", atual.nome);
    printf("Codigo do pedido: %d\n", atual.pedido.cod);

    // exibe os ingredientes da receita do pedido
    printf("Ingredientes do pedido: ");
    imprimeLista(&j->receita);

    printf("--=--=--=--=--=--=--=--=--=--=--=--\n\n");
}

// mostra o pedido, a montagem atual e o proximo ingrediente esperado
static void exibeProgressoMontagem(const Jogo *j) {
    const NodoLista *ingrediente = j->receita.ini;
    int adicionados = tamanhoPilha(&j->prato);
    int i;

    printf("\n------ PROGRESSO DA MONTAGEM ------\n");

    // mostra a sequencia completa do pedido
    printf("Pedido:   ");
    imprimeLista(&j->receita);

    // mostra os ingredientes que ja estao no hamburguer
    printf("Montagem: ");
    imprimePilha(&j->prato);

    // avanca na receita a quantidade de ingredientes ja adicionados
    for (i = 0; i < adicionados && ingrediente != NULL; i++) {
        ingrediente = ingrediente->prox;
    }

    printf("-----------------------------------\n");
}

// exibe informacoes do jogo
static void exibeEstado(const Jogo *j){
    printf("\n--=--=--=-- DADO BURGER --=--=--=--\n");

    printf("Turno: %d\n", j->turno);

    printf("Clientes atendidos: %d/%d\n",
           j->atendidos, j->totalClientes);

    printf("Paciencia restante: %d/%d\n",
           j->paciencia, j->pacienciaMax);

    printf("Pontuacao: %d\n", j->pontuacao);

    printf("--=--=--=--=--=--=--=--=--=--=--=--=--=\n");
}

// traduz os retornos para mensagens que o usuario possa ler
static void exibeResultado(ResultadoAcao resultado){
    switch(resultado){
        case ACAO_SUCESSO:
            printf("Acao concluida\n");
            break;
        case ACAO_SIMBOLO_INVALIDO:
            printf("\nSimbolo invalido!\n");
            break;
        case ACAO_SEM_ESTOQUE:
            printf("\nEstoque vazio!\n");
            break;
        case ACAO_ESTOQUE_CHEIO:
            printf("\nO estoque desse ingrediente esta cheio!\n");
            break;
        case ACAO_PRATO_FECHADO:
            printf("\nO hamburguer ja esta fechado!\n");
            break;
        case ACAO_PRATO_VAZIO:
            printf("\nNao ha ingredientes para remover!\n");
            break;
        case ACAO_JOGO_ENCERRADO:
            printf("\nA partida nao esta em andamento.\n");
            break;
        case ACAO_ERRO_MEMORIA:
            printf("\nErro de alocacao de memoria.\n");
            break;
        case ACAO_ERRO_INTERNO:
            printf("\nOcorreu um erro interno.\n");
            break;
    }
} 

// exibe os ingredientes disponiveis e suas caracteristicas
static void exibeTabelaIngredientes(void) {
    printf("\n==================== INGREDIENTES ====================\n");
    printf("%-18s %-8s %-10s %-10s\n",
           "INGREDIENTE", "SIMBOLO", "PREPARO", "PERECIVEL");
    printf("------------------------------------------------------\n");

    printf("%-18s %-8c %-10s %-10s\n", "Pao (de cima)", 'z', "Nao", "Nao");
    printf("%-18s %-8c %-10s %-10s\n", "Ketchup", 'k', "Nao", "Nao");
    printf("%-18s %-8c %-10s %-10s\n", "Mostarda", 'm', "Nao", "Nao");
    printf("%-18s %-8c %-10s %-10s\n", "Picles", 'p', "Nao", "Nao");
    printf("%-18s %-8c %-10s %-10s\n", "Alface", 'a', "Nao", "Nao");
    printf("%-18s %-8c %-10s %-10s\n", "Queijo suico", 's', "Sim", "Nao");
    printf("%-18s %-8c %-10s %-10s\n", "Queijo cheddar", 'q', "Sim", "Nao");
    printf("%-18s %-8c %-10s %-10s\n", "Cebola", 'c', "Sim", "Nao");
    printf("%-18s %-8c %-10s %-10s\n", "Tomate", 't', "Sim", "Nao");
    printf("%-18s %-8c %-10s %-10s\n", "Carne", 'n', "Sim", "Sim");
    printf("%-18s %-8c %-10s %-10s\n", "Ovo frito", 'o', "Sim", "Sim");
    printf("%-18s %-8c %-10s %-10s\n", "Bacon", 'b', "Sim", "Sim");

    printf("======================================================\n");
}

// exibe as instrucoes e regras do Dado Burger
static void exibeInstrucoes(void) {
    printf("\n========== COMO JOGAR ==========\n\n");

    printf("OBJETIVO DO JOGO:\n");
    printf("Voce e o mais novo funcionario do Dado Burger e devera atender\n");
    printf("aos pedidos de uma serie de clientes ao longo do dia.\n\n");

    // mostra a tabela de ingredientes e seus simbolos
    exibeTabelaIngredientes();
    printf("\n");

    printf("ACOES DISPONIVEIS:\n");
    printf("Preparar um ingrediente: consome um turno.\n");
    printf("Adicionar um ingrediente ao hamburguer: consome um turno.\n");
    printf("Remover o ingrediente do topo do hamburguer: consome um turno.\n");
    printf("Acoes de consulta (ver estoque, ver pedido, etc.): nao consomem turno.\n\n");

    printf("CONDICOES DE VITORIA E DERROTA:\n");
    printf("Vitoria: atender todos os 10 clientes antes que percam a paciencia.\n");
    printf("Derrota: deixar a paciencia de um cliente chegar a zero.\n");
    printf("Abandono: escolher a opcao 0 durante a partida.\n\n");

    printf("PONTUACAO:\n");
    printf("Cada cliente atendido rende %d pontos.\n", RECOMPENSA_CLIENTE);
    printf("Voce tambem recebe %d pontos por unidade de paciencia restante.\n",
           BONUS_PACIENCIA);
    printf("Quanto mais rapido atender os clientes, maior sera sua pontuacao!\n\n");

    printf("INGREDIENTES PERECIVEIS:\n");
    printf("Carne, ovo e bacon possuem prazo de validade apos o preparo.\n");
    printf("Ingredientes vencidos sao descartados automaticamente do estoque.\n");
    printf("Planeje seus preparos para evitar desperdicios!\n\n");
}

// permite consultar o historico de partidas e escolher a ordenacao
static void exibeHistorico(ListaContig *hist) {
    int ordem = 0;
    int leitura;
    int porPontuacao = 0;
    double tOrd = 0.0;

    const char *nomeAlgoritmo = "Quicksort (alfabetico)";

    // pergunta como o jogador deseja ordenar o historico
    printf("Ordenar por: 1 = ordem alfabetica, 2 = pontuacao decrescente: ");

    leitura = leInteiro(&ordem);

    // verifica se a opcao informada e valida
    if (leitura != 1 || (ordem != 1 && ordem != 2)) {
        if (leitura != -1)
            printf("\nOPCAO INVALIDA\n");

        return;
    }

    // configura a ordenacao por pontuacao, caso seja escolhida
    if (ordem == 2) {
        porPontuacao = 1;
        nomeAlgoritmo = "Quicksort (pontuacao)";
    }

    // cria uma copia do historico para testar o mergesort
    // sem alterar os dados da lista original
    ListaContig base = *hist;
    base.dados = NULL;

    if (hist->tamanho > 0) {
        base.dados = (NovoHistorico *) malloc(
            hist->tamanho * sizeof(NovoHistorico)
        );

        if (base.dados != NULL) {
            memcpy(
                base.dados,
                hist->dados,
                hist->tamanho * sizeof(NovoHistorico)
            );
        }
    }

    // exibe o historico ordenado pelo quicksort
    int tam = mostrarHistorico(hist, porPontuacao, &tOrd);

    if (tam < 0) {
        printf("Erro ao ordenar o historico.\n");
    }
    else {
        // registra as metricas da ordenacao
        if (!metricasLogOrdenacao("medicoes.log", nomeAlgoritmo, tam, tOrd)) {
            printf("Aviso: nao foi possivel salvar as metricas da ordenacao.\n");
        }

        // compara o desempenho com o mergesort usando a copia
        if (base.dados != NULL) {
            if (porPontuacao) {
                testarOrdenacao(
                    "Mergesort (pontuacao)",
                    mergesortPontuacao,
                    &base
                );
            }
            else {
                testarOrdenacao(
                    "Mergesort (alfabetico)",
                    mergesortAlfabetico,
                    &base
                );
            }
        }
    }

    // libera a memoria utilizada pela copia do historico
    free(base.dados);
}

static void executaNovoJogo(NodoAvl *raiz, ListaContig *hist, const char *arquivoHistorico, const MetricasCarga *carga){
    Jogo jogo;
    char nome[50];
    int opcao;
    char simbolo;
    int leitura;
    int pedidoConsultado = 0;
    ResultadoAcao resultado;

    // pede o nome do usuario para iniciar a partida
    if (!leNomeJogador(nome, 50)) {
        printf("Nome invalido. Partida cancelada.\n");
        return;
    }

    // inicializa o jogo
    if (!iniciaJogo(&jogo, raiz, MAX_CLIENTES, MAX_PACIENCIA)) {
        printf("Erro ao inicializar a partida.\n");
        return;
    }

    printf("Boa sorte, %s!\n", nome);

    while (jogo.status == JOGO_ANDAMENTO) {
        // mostra o estado atual da partida
        exibeEstado(&jogo);

        // mostra o progresso da montagem depois da primeira consulta ao pedido
        if (pedidoConsultado == 1)
            exibeProgressoMontagem(&jogo);

        // mostra os ingredientes e seus simbolos
        exibeTabelaIngredientes();

        // apresenta as acoes disponiveis
        printf("\n---------- MENU ----------\n");
        printf("1 - Preparar ingrediente\n");
        printf("2 - Adicionar ingrediente ao hamburguer\n");
        printf("3 - Remover ingrediente do hamburguer\n");
        printf("4 - Consultar estoque\n");
        printf("5 - Consultar pedido atual\n");
        printf("0 - Abandonar partida\n");
        printf("--------------------------\n");

        leitura = leInteiro(&opcao); // entrada da acao escolhida

        // se a entrada terminou, abandona a partida e encerra o menu
        if (leitura == -1) {
            jogoAbandona(&jogo);
            break;
        }

        // se a entrada for invalida, permite tentar novamente
        if (leitura == 0) {
            printf("\nOpcao invalida.\n");
            continue;
        }

        // executa a operacao da opcao escolhida
        switch(opcao){
            case 1:
            case 2:
                leitura = leSimbolo(&simbolo);

                // se a entrada terminou, encerra a partida
                if (leitura == 0) {
                    jogoAbandona(&jogo);
                    break;
                }

                // se o simbolo for invalido, permite tentar novamente
                if (leitura == -1) {
                    printf("Entrada de simbolo invalida.\n");
                    break;
                }

                // chama a funcao correspondente no modulo jogo
                if (opcao == 1)
                    resultado = jogoPrepara(&jogo, simbolo);
                else
                    resultado = jogoAdiciona(&jogo, simbolo);

                // exibe o resultado retornado pela operacao.
                exibeResultado(resultado);
                break;

            case 3:
                // remove o item de cima do prato
                resultado = jogoRemove(&jogo);
                exibeResultado(resultado);
                break;
            
            case 4:
                // consulta o estoque
                exibeEstoque(&jogo.estoque);
                break;
            case 5:
                // consulta o pedido
                exibePedido(&jogo);
                // muda o estado da flag
                pedidoConsultado = 1;
                break;
            case 0:
                // encerra a partida
                resultado = jogoAbandona(&jogo);
                exibeResultado(resultado);
                break;
            default:
                printf("Opcao invalida.\n");
                break;

        }

    }

    // fim do while = fim da partida
    printf("\n--=--=--=--FIM DE JOGO--=--=--=--\n");

    switch(jogo.status){
        case JOGO_VITORIA:
            printf("Parabens! Todos os clientes foram atendidos a tempo\n");
            break;
        case JOGO_DERROTA:
            printf("Voce perdeu! Cliente cansou de esperar...\n");
            break;
        case JOGO_ABANDONO:
            printf("Partida abandonada.\n");
            break;
        default:
            break;
    }

    printf("Jogador: %s\n", nome);

    printf("Clientes atendidos: %d/%d\n",
           jogo.atendidos, jogo.totalClientes);

    printf("Pontuacao final: %d\n", jogo.pontuacao);

    // registro do resultado
    if (salvarPartida(arquivoHistorico, hist, nome, jogo.pontuacao))
        printf("Partida registrada no historico.\n");
    else
        printf("Nao foi possivel salvar a partida no historico.\n");

    // montagem do relatorio da partida
    MetricasHash metricasHash = {
        jogo.cache.m,
        jogo.cache.n,
        jogo.cache.hits,
        jogo.cache.misses,
        jogo.cache.colisoes
    };
    
    // tenta salvar as metricas coletadas durante a partida
    if (!metricasRelatorioPartida(
            "medicoes.log",
            carga,
            &jogo.buscas,
            &metricasHash
        )) {

        // avisa caso nao seja possivel gravar o relatorio
        printf("Nao foi possivel salvar as metricas da partida.\n");
    }

    // libera a memoria utilizada pelas estruturas do jogo
    destroiJogo(&jogo);

    // aguarda o jogador pressionar ENTER antes de voltar ao menu.
    printf("\nPressione ENTER para voltar ao menu...\n");
    getchar();
}

// inicializa as estruturas e carrega os dados necessarios para o programa
// retorna 1 se tudo deu certo e 0 se ocorreu algum erro
static int inicializaPrograma(const char *arquivoPedidos, const char *arquivoHistorico,
                             NodoAvl **raiz, ListaContig *hist, MetricasCarga *carga) {

    // inicializa o gerador de numeros aleatorios
    srand((unsigned)time(NULL));

    // comeca com a arvore vazia
    *raiz = NULL;

    // inicializa a lista do historico
    inicializaListaContig(hist);

    // zera as metricas da AVL antes do carregamento
    avlZerarMetricas();

    // mede o tempo necessario para carregar os pedidos
    Timer t;
    timer_iniciar(&t);

    int lidos = carregarPedidos(arquivoPedidos, raiz);

    double tempo = timer_parar(&t);

    // verifica se ocorreu algum erro durante o carregamento
    if (lidos < 0) {
        if (lidos == -1)
            printf("Erro ao abrir arquivo de pedidos: %s\n", arquivoPedidos);
        else
            printf("Erro: arquivo de pedidos invalido ou leitura interrompida.\n");

        return 0;
    }

    // obtem as metricas da arvore AVL
    int alturaAvl = avlAltura(*raiz);
    Contador metricasAVL = avlObterMetricas();

    printf("Pedidos carregados: %d | Tempo: %.6f s\n", lidos, tempo);
    printf("Altura da arvore: %d\n", alturaAvl);

    // carrega os resultados de partidas anteriores
    int nHist = carregarHistorico(arquivoHistorico, hist);

    if (nHist == 0)
        printf("Historico vazio ou nao encontrado.\n");

    // armazena as metricas de carregamento
    *carga = (MetricasCarga) {
        metricasAVL.nosInserido,
        alturaAvl,
        metricasAVL.rotacoes,
        tempo,
        nHist
    };

    // registra as metricas no arquivo de log
    if (!metricasLogCarga("medicoes.log", carga))
        printf("Aviso: nao foi possivel gravar medicoes.log\n");

    return 1;
}

// exibe as opcoes disponiveis no menu principal
static void exibeMenuPrincipal(void) {
    printf("---- ESCOLHA UMA OPCAO ----\n\n");
    printf(" NOVO JOGO --> 1\n");
    printf(" COMO JOGAR --> 2\n");
    printf(" HISTORICO --> 3\n");
    printf(" SAIR --> 0\n");
}

int main(int argc, char *argv[]) {
    // verifica se os arquivos foram informados
    if (argc < 3) {
        printf("Argumentos passados incorretamente.\n");
        printf("Correto: %s <pedidos> <historico>\n", argv[0]);
        return 1;
    }

    NodoAvl *raiz = NULL;
    ListaContig hist;
    MetricasCarga carga;

    // carrega os dados e inicializa as estruturas
    if (!inicializaPrograma(argv[1], argv[2], &raiz, &hist, &carga)) {

        // libera as estruturas em caso de erro
        avlDestruir(raiz);
        destroiListaContig(&hist);

        return 1;
    }

    int opcao = -1;

    do {
        // mostra as opcoes do menu principal
        exibeMenuPrincipal();
        printf("-> ");

        int resultado = leInteiro(&opcao);

        // se a entrada terminou, encerra o menu
        if (resultado == -1)
            break;

        // se o usuario digitou algo invalido, pede outra opcao
        if (resultado == 0) {
            printf("Opcao invalida!\n");
            continue;
        }
        
        printf("\n");

        switch (opcao){
            case 1:
                // inicia uma nova partida com os pedidos ja carregados
                // o historico e as metricas sao passados para registrar os resultados no fim da partida
                executaNovoJogo(raiz, &hist, argv[2], &carga);
                break;
            case 2:
                // exibe o "tutorial" do jogo
                exibeInstrucoes();
                break;
            case 3:
                // permite consultar e ordenar o historico de partidas
                exibeHistorico(&hist);
                break;
            case 0:
                printf("\nSAINDO DO JOGO ... \n");
                break;
            default:
                printf("\n OPCAO INVALIDA \n");

        }

    } while (opcao != 0);

    avlDestruir(raiz);
    destroiListaContig(&hist);

    return 0; 
}