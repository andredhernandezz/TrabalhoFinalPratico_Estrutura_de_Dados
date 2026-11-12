#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ordenacao.h"
#include "timer.h"
#include "metricas.h"



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

/* ================= BUBBLE SORT ================= */

int bubbleSortAlfabetico(ListaContig *lista){
    if (lista == NULL || lista->dados == NULL){
        return -1;
    }

    if (lista->tamanho < 2){
        return 1;
    }

    NovoHistorico *v = lista->dados;
    NovoHistorico temp;
    int n = lista->tamanho;
    int trocou;

    do {
        trocou = 0;
        for (int i = 0; i < n - 1; i++){
            if (vemAntesNome(&v[i + 1], &v[i])){
                temp = v[i];
                v[i] = v[i + 1];
                v[i + 1] = temp;
                trocou = 1;
            }
        }
        n--;
    } while (trocou);

    return 1;
}

int bubbleSortPontuacao(ListaContig *lista){
    if (lista == NULL || lista->dados == NULL){
        return -1;
    }

    if (lista->tamanho < 2){
        return 1;
    }

    NovoHistorico *v = lista->dados;
    NovoHistorico temp;
    int n = lista->tamanho;
    int trocou;

    do {
        trocou = 0;
        for (int i = 0; i < n - 1; i++){
            if (vemAntesPontuacao(&v[i + 1], &v[i])){
                temp = v[i];
                v[i] = v[i + 1];
                v[i + 1] = temp;
                trocou = 1;
            }
        }
        n--;
    } while (trocou);

    return 1;
}

/* ================= MERGE SORT ================= */

void mergeAlfabetico(NovoHistorico *v, NovoHistorico *aux, int esq, int meio, int dir){
    int i = esq;
    int j = meio + 1;
    int k = esq;

    while (i <= meio && j <= dir){
        if (vemAntesNome(&v[j], &v[i])){
            aux[k++] = v[j++];
        } else {
            aux[k++] = v[i++];
        }
    }
    while (i <= meio){
        aux[k++] = v[i++];
    }
    while (j <= dir){
        aux[k++] = v[j++];
    }

    for (k = esq; k <= dir; k++){
        v[k] = aux[k];
    }
}

void mergesortAlfabeticoRec(NovoHistorico *v, NovoHistorico *aux, int esq, int dir){
    if (esq >= dir){
        return;
    }

    int meio = esq + (dir - esq) / 2;
    mergesortAlfabeticoRec(v, aux, esq, meio);
    mergesortAlfabeticoRec(v, aux, meio + 1, dir);
    mergeAlfabetico(v, aux, esq, meio, dir);
}

int mergesortAlfabetico(ListaContig *lista){
    if (lista == NULL || lista->dados == NULL){
        return -1;
    }

    if (lista->tamanho < 2){
        return 1;
    }

    NovoHistorico *aux = (NovoHistorico *) malloc(lista->tamanho * sizeof(NovoHistorico));
    if (aux == NULL){
        return 0;
    }

    mergesortAlfabeticoRec(lista->dados, aux, 0, lista->tamanho - 1);
    free(aux);
    return 1;
}

void mergePontuacao(NovoHistorico *v, NovoHistorico *aux, int esq, int meio, int dir){
    int i = esq;
    int j = meio + 1;
    int k = esq;

    while (i <= meio && j <= dir){
        if (vemAntesPontuacao(&v[j], &v[i])){
            aux[k++] = v[j++];
        } else {
            aux[k++] = v[i++];
        }
    }
    while (i <= meio){
        aux[k++] = v[i++];
    }
    while (j <= dir){
        aux[k++] = v[j++];
    }

    for (k = esq; k <= dir; k++){
        v[k] = aux[k];
    }
}

void mergesortPontuacaoRec(NovoHistorico *v, NovoHistorico *aux, int esq, int dir){
    if (esq >= dir){
        return;
    }

    int meio = esq + (dir - esq) / 2;
    mergesortPontuacaoRec(v, aux, esq, meio);
    mergesortPontuacaoRec(v, aux, meio + 1, dir);
    mergePontuacao(v, aux, esq, meio, dir);
}

int mergesortPontuacao(ListaContig *lista){
    if (lista == NULL || lista->dados == NULL){
        return -1;
    }

    if (lista->tamanho < 2){
        return 1;
    }

    NovoHistorico *aux = (NovoHistorico *) malloc(lista->tamanho * sizeof(NovoHistorico));
    if (aux == NULL){
        return 0;
    }

    mergesortPontuacaoRec(lista->dados, aux, 0, lista->tamanho - 1);
    free(aux);
    return 1;
}


static void testarOrdenacao(const char *nome, int (*alg)(ListaContig *), const ListaContig *base) {
    ListaContig copia = *base;
    copia.dados = (NovoHistorico *) malloc(base->tamanho * sizeof(NovoHistorico));
    if (copia.dados == NULL) {
        printf("Erro de memoria ao testar %s.\n", nome);
        return;
    }
    memcpy(copia.dados, base->dados, base->tamanho * sizeof(NovoHistorico));

    Timer tm;
    timer_iniciar(&tm);
    int r = alg(&copia);
    double seg = timer_parar(&tm);

    if (r != 1)
        printf("Erro ao ordenar com %s.\n", nome);
    else {
        printf("%s | n = %d | Tempo: %.6f s\n", nome, base->tamanho, seg);
        metricasLogOrdenacao("medicoes.log", nome, base->tamanho, seg);
    }
    free(copia.dados);
}
