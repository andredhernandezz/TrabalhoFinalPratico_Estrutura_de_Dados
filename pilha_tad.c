#include <stdio.h>
#include <stdlib.h>
#include "pilha_tad.h"

void inicializaPilha(PilhaEnc *p) {
    if (p == NULL)
        return;

    p->topo = NULL;
    p->tamanho = 0;
}

int pilhaVazia(const PilhaEnc *p) {
    if (p == NULL)
        return -1;

    return p->topo == NULL;
}

int topoPilha(const PilhaEnc *p, char *ingrediente) {
    if (p == NULL || ingrediente == NULL)
        return -1; 
    if (pilhaVazia(p))
        return 0;

    *ingrediente = p->topo->ingrediente;
    return 1;
}

int push(PilhaEnc *p, char ingrediente) {
    if (p == NULL)
        return -1;

    NodoPilha *novo = (NodoPilha*) malloc(sizeof(NodoPilha));
    if (novo == NULL)
        return 0; 

    novo->ingrediente = ingrediente;
    novo->prox = p->topo;
    p->topo = novo;
    p->tamanho++;
    return 1;
}

int pop(PilhaEnc *p, char *ingrediente) {
    if (p == NULL || ingrediente == NULL)
        return -1;
    if (pilhaVazia(p))
        return 0;

    NodoPilha *removido = p->topo;
    *ingrediente = removido->ingrediente;
    p->topo = removido->prox;
    p->tamanho--;

    free(removido);
    return 1;
}

int tamanhoPilha(const PilhaEnc *p) {
    if (p == NULL)
        return -1;

    return p->tamanho;
}


void imprimeRec(const NodoPilha *no) {
    if (no == NULL)
        return;
    imprimeRec(no->prox);
    printf("%c", no->ingrediente);
}

void imprimePilha(const PilhaEnc *p) {
    if (p == NULL) {
        printf("ERRO em imprimePilha: pilha não inicializada.\n");
        return;
    }

    if (pilhaVazia(p)) {
        printf("(prato vazio)\n");
        return;
    }

    imprimeRec(p->topo);
    printf("\n");
}

int confereRec(const NodoPilha *no, const NodoLista **cursor) {
    if (no == NULL)
        return 1; 

    if (!confereRec(no->prox, cursor))
        return 0;

    if (*cursor == NULL) 
        return 0;
    if ((*cursor)->ingrediente != no->ingrediente)
        return 0;

    *cursor = (*cursor)->prox;
    return 1;
}

int pilhaConfereReceita(const PilhaEnc *p, const ListaEnc *receita) {
    if (p == NULL || receita == NULL)
        return -1;

    const NodoLista *cursor = receita->ini;

    if (!confereRec(p->topo, &cursor))
        return 0;

    return cursor == NULL; 
}

void esvaziaPilha(PilhaEnc *p) {
    if (p == NULL) {
        printf("ERRO em esvaziaPilha: pilha não inicializada.\n");
        return;
    }

    NodoPilha *aux = p->topo;
    NodoPilha *temp;

    while (aux != NULL) {
        temp = aux;
        aux = aux->prox;
        free(temp);
    }

    p->topo = NULL;
    p->tamanho = 0;
}