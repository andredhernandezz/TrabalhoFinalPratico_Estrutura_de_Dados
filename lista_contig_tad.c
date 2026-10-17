#include <stdio.h>
#include <stdlib.h>
#include "lista_contig_tad.h"

#define CAP_INICIAL 20



int inicializaListaContig(ListaContig *lista){
    if (lista == NULL){
        return -1;
    }
    lista->tamanho = 0;
    lista->capacidade = 0;
    lista->dados = (NovoHistorico*) malloc(CAP_INICIAL *sizeof(NovoHistorico));
    if (lista->dados == NULL){
        return 0;
    }

    lista->capacidade = CAP_INICIAL;
    return 1;
}


int insereFimContig(ListaContig *lista, NovoHistorico input){
    if (lista == NULL || lista->dados == NULL){
        return -1;
    }

    if (lista->tamanho == lista->capacidade){
        int novaCapacidade = lista->capacidade * 2;
        NovoHistorico *novo = (NovoHistorico*) realloc(lista->dados, novaCapacidade * sizeof(NovoHistorico));
        if (novo == NULL){
            return 0;
        }
        lista->dados = novo;
        lista->capacidade = novaCapacidade;
    }
    lista->dados[lista->tamanho] = input;
    lista->tamanho++;
    return 1;

}



int obtemContig(const ListaContig *lista, int indice, NovoHistorico *novo){
    if (lista == NULL || lista->dados == NULL || novo == NULL){
        return -1;
    }
    if (indice < 0 || indice >= lista->tamanho){
        return 0;
    }

    *novo = lista->dados[indice];
    return 1;
}


int tamanhoListaContig(const ListaContig *lista){
    if (lista == NULL || lista->dados == NULL){
        return -1;
    }
    return lista->tamanho;
}


int copiaListaContig(const ListaContig *origem, ListaContig *destino){ // para ordenacao 
    if (origem == NULL || destino == NULL || origem->dados == NULL){
        return -1;
    }
    int capacidade = origem->tamanho;
    if (capacidade == 0){
        capacidade = 1;
    }

    NovoHistorico *dados = (NovoHistorico*) malloc(capacidade * sizeof(NovoHistorico));
    if (dados == NULL){
        return 0;
    }
    for (int i = 0; i < origem->tamanho; i++){
        dados[i] = origem->dados[i];
    }

    destino->dados = dados;
    destino->tamanho = origem->tamanho;
    destino->capacidade = capacidade;
    return 1;
}


void destroiListaContig(ListaContig *lista){
    if (lista == NULL){
        return;
    }
    free(lista->dados);
    lista->dados = NULL;
    lista->tamanho = 0;
    lista->capacidade = 0;
}