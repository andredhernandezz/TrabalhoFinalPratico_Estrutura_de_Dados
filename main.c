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

// exibe informacoes do jogo
static void exibeEstado(const Jogo *j){
    printf("\n--=--=--=-- DADO BURGER --=--=--=--\n");

    printf("Turno: %d\n", j->turno);

    printf("Clientes atendidos: %d/%d\n",
           j->atendidos, j->totalClientes);

    printf("Paciencia restante: %d/%d\n",
           j->paciencia, j->pacienciaMax);

    printf("Pontuacao: %d\n", j->pontuacao);

    // imprime os ingredientes que ja foram adicionados ao prato
    printf("\nHamburguer em montagem: ");
    imprimePilha(&j->prato);

    printf("--=--=--=--=--=--=--=--=--=--=--=--=--=\n");
}

// traduz os retornos para mensagens que o usuario possa ler
static void exibeResultado(ResultadoAcao resultado){
    switch(resultado){
        case ACAO_SUCESSO:
            printf("Acao concluida");
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

static void executaNovoJogo(NodoAvl *raiz, ListaContig *hist, const char *arquivoHistorico, const MetricasCarga *carga){
    Jogo jogo;
    char nome[50];
    int opcao;
    char simbolo;
    int leitura;
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

    while (jogo.status == JOGO_ANDAMENTO){
        exibeEstado(&jogo); // atualiza os dados da partida

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
    printf("--=--=--=--FIM DE JOGO--=--=--=--\n");

    switch(jogo.status){
        case JOGO_VITORIA:
            printf("Parabens! Todos os clientes foram atendidos a tempo\n");
            break;
        case JOGO_DERROTA:
            printf("Voce perdeu! Cliente cansou de esperar...\n");
            break;
        case JOGO_ABANDONO:
            printf("Saindo...\n");
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

int main(int argc, char *argv[]) {

    srand((unsigned)time(NULL)); // inicializa o gerador de números aleatórios
    
    NodoAvl *raiz = NULL;

    if (argc < 3){
        printf("Argumentos passados incorretamente \nCorreto: %s <pedidos> <historico>\n", argv[0]);
        return 1;
    }

    avlZerarMetricas(); // ZERA AS METRICAS DA AVL 

    Timer t;
    timer_iniciar(&t);
    int lidos = carregarPedidos(argv[1], &raiz);
    double s = timer_parar(&t);


    int alturaAvl = avlAltura(raiz);
    Contador metricasAVL;
    metricasAVL = avlObterMetricas();

    // verifica se ocorreu algum erro durante o carregamento
    if (lidos < 0) {

        if (lidos == -1)
            printf("Erro ao abrir arquivo de pedidos: %s\n", argv[1]);
        else
            printf("Erro: arquivo de pedidos invalido ou leitura interrompida.\n");

        // libera os pedidos podem ter sido carregados
        raiz = avlDestruir(raiz);

        return 1;
    }

    printf("Pedidos carregados: %d | Tempo: %.6f s\n", lidos, s);
    printf("Altura da arvore: %d\n", alturaAvl);

    ListaContig hist;
    inicializaListaContig(&hist);
    int nHist = carregarHistorico(argv[2], &hist);
    if (nHist == 0)
        printf("Historico vazio ou nao encontrado.\n");

    MetricasCarga carga = { metricasAVL.nosInserido, alturaAvl, metricasAVL.rotacoes, s, nHist };
    if (!metricasLogCarga("medicoes.log", &carga))
        printf("Aviso: nao foi possivel gravar medicoes.log\n");

    int opcao = -1;
    do {
        printf("---- ESCOLHA UMA OPCAO ----\n\n");
        printf(" NOVO JOGO --> 1\n");
        printf(" COMO JOGAR --> 2\n");
        printf(" HISTORICO --> 3\n");
        printf(" SAIR --> 0\n");
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
                // inicia uma nova partida com os pedidos ja carregados.
                // o historico e as metricas sao passados para registrar os resultados no fim da partida
                executaNovoJogo(raiz, &hist, argv[2], &carga);
                break;
            case 2:
                printf("OBJETIVO DO JOGO: Voce e o mais novo funcionario do jogo Dado Burguer e devera atender aos pedidos de uma serie de clientes ao longo do dia \n\n");
                printf("INGREDIENTE\t\tSIMBOLO\tREQUER PREPARO?\tPERECIVEL?\n");
                printf("Pao (de cima)\t\tz\tNao\t\tNao\n");
                printf("Ketchup\t\t\tk\tNao\t\tNao\n");
                printf("Mostarda\t\tm\tNao\t\tNao\n");
                printf("Picles\t\t\tp\tNao\t\tNao\n");
                printf("Alface\t\t\ta\tNao\t\tNao\n");
                printf("Queijo suico\t\ts\tSim\t\tNao\n");
                printf("Queijo cheddar\t\tq\tSim\t\tNao\n");
                printf("Cebola\t\t\tc\tSim\t\tNao\n");
                printf("Tomate\t\t\tt\tSim\t\tNao\n");
                printf("Carne\t\t\tn\tSim\t\tSim\n");
                printf("Ovo frito\t\to\tSim\t\tSim\n");
                printf("Bacon\t\t\tb\tSim\t\tSim\n\n");
                printf("ACOES DISPONIVEIS:\nPreparar um ingrediente: consome um turno\nAdicionar um ingrediente ao hamburguer: consome um turno\nRemover o ingrediente do topo do hamburguer: consome um turno\nAcoes de consulta(ver estoque, ver pedido, etc.): nao consomem turno \n\n");
                printf("CONDICOES DE VITORIA E DERROTA ... \n\n");
                break;
            case 3:
                {
                    int ordem = 0;
                    printf("Ordenar por: 1 = ordem alfabetica, 2 = pontuacao decrescente: ");
                    int leituraOrdem = leInteiro(&ordem);
                    // se a entrada terminou, encerra o submenu
                    if (leituraOrdem == -1)
                        break;
                    // verifica se a opcao digitada e valida
                    if (leituraOrdem == 0 || (ordem != 1 && ordem != 2)) {
                        printf("\nOPCAO INVALIDA\n");
                        break;
                    }
                    int porPontuacao = 0;
                    const char *nomeAlgoritmo = "Quicksort (alfabetico)";
                    if (ordem == 2) {
                        porPontuacao = 1;
                        nomeAlgoritmo = "Quicksort (pontuacao)";
                    }

                    // NOVO: guarda uma copia da lista antes do quicksort mexer nela
                    ListaContig base = hist;
                    base.dados = NULL;
                    if (hist.tamanho > 0) {
                        base.dados = (NovoHistorico *) malloc(hist.tamanho * sizeof(NovoHistorico));
                        if (base.dados != NULL)
                            memcpy(base.dados, hist.dados, hist.tamanho * sizeof(NovoHistorico));
                    }

                    double tOrd = 0.0;
                    int tam = mostrarHistorico(&hist, porPontuacao, &tOrd);
                    if (tam < 0)
                        printf("Erro ao ordenar o historico.\n");
                    else {
                        // tenta registrar as metricas da ordenacao
                        if (!metricasLogOrdenacao("medicoes.log", nomeAlgoritmo, tam, tOrd)) {
                        // informa caso a gravacao tenha falhado
                        printf("Nao foi possivel salvar as metricas da ordenacao.\n");
                        }

                        if (base.dados != NULL) {
                            if (porPontuacao) {
                                testarOrdenacao("Mergesort (pontuacao)", mergesortPontuacao, &base);
                            } else {
                                testarOrdenacao("Mergesort (alfabetico)", mergesortAlfabetico, &base);
                            }
                        }
                    }
                    free(base.dados); // NOVO
                }
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