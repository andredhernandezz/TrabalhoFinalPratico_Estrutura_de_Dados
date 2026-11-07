#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "avl_tad.h"
#include "carregamento.h"


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

    /* historico: por enquanto so verifica se existe;
       depois sera carregado na lista contigua */
    FILE *historico = fopen(argv[2], "r");
    if (!historico){
        printf("Historico nao encontrado, abrindo um vazio. \n");
    }
    else {
        fclose(historico);
    }

    FILE *log = fopen("medicoes.log", "w");
    if (log){
        fprintf(log, "-- AVL: carregamento --\n");
        fprintf(log, "Tempo de carregamento: %.6f s\n", s);
        fprintf(log, "Nos inseridos: %d\n", metricasAVL.nosInserido);
        fprintf(log, "Altura da AVL: %d\n", alturaAvl);
        fprintf(log, "Numero de rotacoes na AVL: %d\n", metricasAVL.rotacoes);
        


        fclose(log);
    }


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
                printf("\nEXIBINDO HISTORICO: ...\n");
                break;
            case 0:
                printf("\nSAINDO DO JOGO ... \n");
                break;
            default:
                printf("\n OPCAO INVALIDA \n");

        }

    } while (opcao != 0);

    avlDestruir(raiz);
    return 0;
}
