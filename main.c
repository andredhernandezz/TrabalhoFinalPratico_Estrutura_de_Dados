#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* PRA QUANDO EXISTIR A LOGICA DO JOGO:
int buscarPedido(NodoAvl *raiz, TabelaHash *cache, int cod, Pedido *out, MetricasBusca *mb);
int salvarPartida(const char *arq, ListaContig *lista, const char *nome, int pontuacao);
int mostrarHistorico(const ListaContig *lista, int porPontuacao, double *tempo);
*/



int main(int argc, char *argv[]) {
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

    if (lidos < 0){
        printf("Erro ao abrir arquivo de pedidos: %s\n", argv[1]);
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
        scanf("%d", &opcao);
        printf("\n");


        switch (opcao){
            case 1:
                printf("\nINICIANDO NOVO JOGO ...\n");
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
                    if (scanf("%d", &ordem) != 1 || (ordem != 1 && ordem != 2)) {
                        int c;
                        while ((c = getchar()) != '\n' && c != EOF) {}
                        printf("\n OPCAO INVALIDA \n");
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
                        metricasLogOrdenacao("medicoes.log", nomeAlgoritmo, tam, tOrd);

                        // NOVO: mesmos testes de tempo para mergesort e bubble sort
                        if (base.dados != NULL) {
                            if (porPontuacao) {
                                testarOrdenacao("Mergesort (pontuacao)", mergesortPontuacao, &base);
                                testarOrdenacao("Bubble sort (pontuacao)", bubbleSortPontuacao, &base);
                            } else {
                                testarOrdenacao("Mergesort (alfabetico)", mergesortAlfabetico, &base);
                                testarOrdenacao("Bubble sort (alfabetico)", bubbleSortAlfabetico, &base);
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