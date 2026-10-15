#include <stdio.h>
#include <stdlib.h>

int main() {
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
