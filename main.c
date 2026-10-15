#include <stdio.h>
#include <stdlib.h>
#include "timer.h"


// incluir isso aqui depois q ja tiver implementado

// #include "avl.h"
// #include "lista.h"
// #include "hash.h"
// #include "heap.h"
// #include "pilha.h"
// #include "fila.h"
int main(int argc, char *argv[]) {
    
    if (argc < 3){
        printf("Argumentos passados incorretamente \nCorreto: %s <pedidos> <historico>\n", argv[0]);
        return 1;
    }
    
    Timer t;
    timer_iniciar(&t);

    FILE *pedidos = fopen(argv[1], "r");
    if (!pedidos){
        printf("Erro ao abrir arquivo de pedidos: %s\n", argv[1]);
        return 1;
    }

    fclose(pedidos);

    FILE *historico = fopen(argv[2], "r");
    if (!historico){
        printf("Historico nao encontrado, abrindo um vazio. \n");
    }
    else {
        fclose(historico);
    }

    double s = timer_parar(&t);
    printf("Tempo: %.6f s\n", s);

    FILE *log = fopen("medicoes.log", "w");
    if (log){
        fprintf(log, "-- METRICAS INICIAIS --  \n");
        fprintf(log, "Tempo de carregamento: %.2f s\n", s);

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
                printf("OBJETIVO DO JOGO: ...\n");
                printf("INGREDIENTES E SIMBOLOS ... \n");
                printf("ACOES DISPONIVEIS ... \n");
                printf("CONIDCOES DE VITORIA E DERROTA ... \n");
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

    
    return 0;
}
