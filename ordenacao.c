#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ordenacao.h"
#include "timer.h"
#include "metricas.h"


// QUICKSORT

// compara dois registros por ordem alfabetica, usando a maior pontuacao como desempate
// retorna 1 se a deve vir antes de b e 0 caso contrario
int vemAntesNome(const NovoHistorico *a, const NovoHistorico *b) {
    int cmp = strcmp(a->nome, b->nome);
    if (cmp != 0)
        return cmp < 0;
    return a->pontuacao > b->pontuacao;
}

// implementa recursivamente o quicksort utilizando a ordem alfabetica
void quicksortAlfabeticoRec(NovoHistorico *v, int esq, int dir){
    int i = esq;
    int j = dir;

    // escolhe o elemento da posicao central como pivo
    NovoHistorico pivo = v[(esq + dir) / 2];
    NovoHistorico temp;

    // particiona o vetor de acordo com o pivo
    while (i <= j){
        while (vemAntesNome(&v[i], &pivo)){
            i++;
        }
        while (vemAntesNome(&pivo, &v[j])){
            j--;
        }

        // troca os elementos que estao fora da ordem desejada
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

// ordena a lista contigua alfabeticamente utilizando quicksort
// retorna 1 se sucesso e -1 se lista invalida
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

// compara dois registros por pontuacao decrescente, usando o nome como desempate
// retorna 1 se a deve vir antes de b e 0 caso contrario
int vemAntesPontuacao(const NovoHistorico *a, const NovoHistorico *b){
    if (a->pontuacao != b->pontuacao){
        return a->pontuacao > b->pontuacao;
    }
    return strcmp(a->nome, b->nome) < 0;
}

// implementa recursivamente o quicksort utilizando a ordem decrescente de pontuacao
void quicksortPontuacaoRec(NovoHistorico *v, int esq, int dir){
    int i = esq;
    int j = dir;

    // escolhe o elemento da posicao central como pivo
    NovoHistorico pivo = v[(esq + dir) / 2];
    NovoHistorico temp;

    // particiona o vetor de acordo com o pivo
    while (i<=j){
        while (vemAntesPontuacao(&v[i], &pivo)){
            i++;
        }
        while (vemAntesPontuacao(&pivo, &v[j])){
            j--;
        }

        // troca os elementos que estao fora da ordem desejada
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

// ordena a lista contigua por pontuacao decrescente utilizando quicksort
// retorna 1 se sucesso e -1 se lista invalida
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

// MERGESORT

// intercala duas partes previamente ordenadas do vetor utilizando a ordem alfabetica
void mergeAlfabetico(NovoHistorico *v, NovoHistorico *aux, int esq, int meio, int dir){
    int i = esq;
    int j = meio + 1;
    int k = esq;

    // compara os elementos das duas partes e insere o proximo na ordem correta
    while (i <= meio && j <= dir){
        if (vemAntesNome(&v[j], &v[i])){
            aux[k++] = v[j++];
        } else {
            aux[k++] = v[i++];
        }
    }
    // copia os elementos restantes de cada parte para o vetor auxiliar
    while (i <= meio){
        aux[k++] = v[i++];
    }
    while (j <= dir){
        aux[k++] = v[j++];
    }

    // copia o trecho ordenado de volta para o vetor original
    for (k = esq; k <= dir; k++){
        v[k] = aux[k];
    }
}

// implementa recursivamente o mergesort utilizando a ordem alfabetica
void mergesortAlfabeticoRec(NovoHistorico *v, NovoHistorico *aux, int esq, int dir){
    // caso base: trechos com no maximo um elemento ja estao ordenados
    if (esq >= dir){
        return;
    }

    // divide o vetor em duas partes e ordena cada uma recursivamente
    int meio = esq + (dir - esq) / 2;
    mergesortAlfabeticoRec(v, aux, esq, meio);
    mergesortAlfabeticoRec(v, aux, meio + 1, dir);
    // intercala as duas partes ordenadas
    mergeAlfabetico(v, aux, esq, meio, dir);
}

// ordena a lista contigua alfabeticamente utilizando mergesort
// retorna 1 se sucesso, 0 se falha de alocacao e -1 se lista invalida
int mergesortAlfabetico(ListaContig *lista){
    if (lista == NULL || lista->dados == NULL){
        return -1;
    }

    if (lista->tamanho < 2){
        return 1;
    }

    // aloca um vetor auxiliar de tamanho n, reutilizado durante toda a ordenacao
    NovoHistorico *aux = (NovoHistorico *) malloc(lista->tamanho * sizeof(NovoHistorico));
    if (aux == NULL){
        return 0;
    }

    mergesortAlfabeticoRec(lista->dados, aux, 0, lista->tamanho - 1);
    free(aux);
    return 1;
}

// intercala duas partes previamente ordenadas do vetor por pontuacao decrescente
void mergePontuacao(NovoHistorico *v, NovoHistorico *aux, int esq, int meio, int dir){
    int i = esq;
    int j = meio + 1;
    int k = esq;

    // compara os elementos das duas partes e insere o proximo na ordem correta
    while (i <= meio && j <= dir){
        if (vemAntesPontuacao(&v[j], &v[i])){
            aux[k++] = v[j++];
        } else {
            aux[k++] = v[i++];
        }
    }
    // copia os elementos restantes de cada parte para o vetor auxiliar
    while (i <= meio){
        aux[k++] = v[i++];
    }
    while (j <= dir){
        aux[k++] = v[j++];
    }

    // copia o trecho ordenado de volta para o vetor original
    for (k = esq; k <= dir; k++){
        v[k] = aux[k];
    }
}

// implementa recursivamente o mergesort utilizando a ordem decrescente de pontuacao
void mergesortPontuacaoRec(NovoHistorico *v, NovoHistorico *aux, int esq, int dir){
    // caso base: trechos com no maximo um elemento ja estao ordenados
    if (esq >= dir){
        return;
    }

    // divide o vetor em duas partes e ordena cada uma recursivamente
    int meio = esq + (dir - esq) / 2;
    mergesortPontuacaoRec(v, aux, esq, meio);
    mergesortPontuacaoRec(v, aux, meio + 1, dir);
    // intercala as duas partes ordenadas
    mergePontuacao(v, aux, esq, meio, dir);
}

// ordena a lista contigua por pontuacao decrescente utilizando mergesort
// retorna 1 se sucesso, 0 se falha de alocacao e -1 se lista invalida
int mergesortPontuacao(ListaContig *lista){
    if (lista == NULL || lista->dados == NULL){
        return -1;
    }

    if (lista->tamanho < 2){
        return 1;
    }

    // aloca um vetor auxiliar de tamanho n, reutilizado durante toda a ordenacao
    NovoHistorico *aux = (NovoHistorico *) malloc(lista->tamanho * sizeof(NovoHistorico));
    if (aux == NULL){
        return 0;
    }

    mergesortPontuacaoRec(lista->dados, aux, 0, lista->tamanho - 1);
    free(aux);
    return 1;
}

// executa um algoritmo de ordenacao sobre uma copia do historico
// mede o tempo de execucao e registra o resultado em medicoes.log
void testarOrdenacao(const char *nome, int (*alg)(ListaContig *), const ListaContig *base) {
    // cria uma copia independente para preservar os registros originais
    ListaContig copia = *base;
    copia.dados = (NovoHistorico *) malloc(base->tamanho * sizeof(NovoHistorico));
    if (copia.dados == NULL) {
        printf("Erro de memoria ao testar %s.\n", nome);
        return;
    }
    memcpy(copia.dados, base->dados, base->tamanho * sizeof(NovoHistorico));

    // cronometra apenas a execucao do algoritmo de ordenacao
    Timer tm;
    timer_iniciar(&tm);
    int r = alg(&copia);
    double seg = timer_parar(&tm);

    if (r != 1)
        printf("Erro ao ordenar com %s.\n", nome);
    else {
        // registra o algoritmo, a quantidade de registros e o tempo medido
        metricasLogOrdenacao("medicoes.log", nome, base->tamanho, seg);
    }
    free(copia.dados);
}