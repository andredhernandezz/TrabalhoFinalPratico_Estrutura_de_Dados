#include <stdio.h>
#include <string.h>
#include "ordenacao.h"




int vemAntesNome(const NovoHistorico *a, const NovoHistorico *b) {
    int cmp = strcmp(a->nome, b->nome);
    if (cmp != 0)
        return cmp < 0;
    return a->pontuacao > b->pontuacao;
}

void quicksortAlfabeticoRec(NovoHistorico *v, int esq, int dir){
    int i = esq;
    int j = dir;

    NovoHistorico pivo = v[(esq + dir) / 2];
    NovoHistorico temp;

    while (i <= j){
        while (vemAntesNome(&v[i], &pivo)){
            i++;
        }
        while (vemAntesNome(&pivo, &v[j])){
            j--;
        }

        if (i<=j){
            temp = v[i];
            v[i] = v[j];
            v[j] = temp;
            i++;
            j--;
        }
    }

    if (esq < j){
        quicksortAlfabeticoRec(v, esq, j);
    }
    if (i < dir){
        quicksortAlfabeticoRec(v, i , dir);
    }
}

int quicksortAlfabetico(ListaContig *lista){
    if (lista == NULL || lista->dados == NULL){
        return -1;
    }

    if (lista->tamanho < 2){
        return 1;
    }

    quicksortAlfabeticoRec(lista->dados, 0, lista->tamanho - 1);
    return 1;
}





int vemAntesPontuacao(const NovoHistorico *a, const NovoHistorico *b){
    if (a->pontuacao != b->pontuacao){
        return a->pontuacao > b->pontuacao;
    }
    return strcmp(a->nome, b->nome) < 0;
}

void quicksortPontuacaoRec(NovoHistorico *v, int esq, int dir){
    int i = esq;
    int j = dir;

    NovoHistorico pivo = v[(esq + dir) / 2];
    NovoHistorico temp;

    while (i<=j){
        while (vemAntesPontuacao(&v[i], &pivo)){
            i++;
        }
        while (vemAntesPontuacao(&pivo, &v[j])){
            j--;
        }

        if (i<=j){
            temp = v[i];
            v[i] = v[j];
            v[j] = temp;
            i++;
            j--;
        }
    }

    if (esq < j){
        quicksortPontuacaoRec(v, esq, j);
    }
    if (i < dir){
        quicksortPontuacaoRec(v, i, dir);
    }
}

int quicksortPontuacao(ListaContig *lista){
    if (lista == NULL || lista->dados == NULL){
        return -1;
    }

    if (lista->tamanho < 2){
        return 1;
    }

    quicksortPontuacaoRec(lista->dados, 0, lista->tamanho - 1);
    return 1;
}

