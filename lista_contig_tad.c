#include <stdio.h>
#include <stdlib.h>
#include "lista_contig_tad.h"

#define CAP_INICIAL 20 // capacidade inicial do vetor

// inicializa uma lista contigua vazia e aloca o vetor inicial
// retorna 1 se sucesso, 0 se falha de alocacao e -1 se lista invalida
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

// insere um registro no final da lista em O(1) amortizado
// retorna 1 se sucesso, 0 se falha de alocacao e -1 se lista invalida
int insereFimContig(ListaContig *lista, NovoHistorico input){
    if (lista == NULL || lista->dados == NULL){
        return -1;
    }

    // dobra a capacidade do vetor quando nao ha mais espaco disponivel
    if (lista->tamanho == lista->capacidade){
        int novaCapacidade = lista->capacidade * 2;
        NovoHistorico *novo = (NovoHistorico*) realloc(lista->dados, novaCapacidade * sizeof(NovoHistorico));
        if (novo == NULL){
            return 0;
        }
        // atualiza o vetor e sua capacidade somente apos o sucesso do realloc
        lista->dados = novo;
        lista->capacidade = novaCapacidade;
    }
    lista->dados[lista->tamanho] = input;
    lista->tamanho++;
    return 1;

}

// consulta um registro pelo indice em O(1)
// retorna 1 se sucesso, 0 se indice fora dos limites e -1 se parametros invalidos
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

// retorna a quantidade de registros armazenados em O(1)
// retorna -1 se a lista for invalida
int tamanhoListaContig(const ListaContig *lista){
    if (lista == NULL || lista->dados == NULL){
        return -1;
    }
    return lista->tamanho;
}

// cria uma copia independente da lista para realizar a ordenacao sem alterar a original
// retorna 1 se sucesso, 0 se falha de alocacao e -1 se parametros invalidos
int copiaListaContig(const ListaContig *origem, ListaContig *destino){
    if (origem == NULL || destino == NULL || origem->dados == NULL){
        return -1;
    }
    int capacidade = origem->tamanho;
    // garante uma capacidade minima mesmo quando a lista original esta vazia
    if (capacidade == 0){
        capacidade = 1;
    }

    NovoHistorico *dados = (NovoHistorico*) malloc(capacidade * sizeof(NovoHistorico));
    if (dados == NULL){
        return 0;
    }
    // copia todos os registros para o novo vetor em O(n)
    for (int i = 0; i < origem->tamanho; i++){
        dados[i] = origem->dados[i];
    }

    destino->dados = dados;
    destino->tamanho = origem->tamanho;
    destino->capacidade = capacidade;
    return 1;
}

// libera o vetor e restaura os campos da lista ao estado vazio
void destroiListaContig(ListaContig *lista){
    if (lista == NULL){
        return;
    }
    free(lista->dados);
    lista->dados = NULL;
    lista->tamanho = 0;
    lista->capacidade = 0;
}