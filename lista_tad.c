#include <stdio.h>
#include <stdlib.h>
#include "lista_tad.h"

void inicializaLista(ListaEnc *lista) {
    if (lista == NULL)
        return;

    lista->ini = NULL;
    lista->fim = NULL;
}

int tamanhoLista(const ListaEnc *lista) {
    if (lista == NULL)
        return -1;

    NodoLista *aux = lista->ini;
    int contador = 0;

    while (aux != NULL) {
        contador++;
        aux = aux->prox;
    }

    return contador;
}

void imprimeLista(const ListaEnc *lista) {
    if (lista == NULL)
        return;

    NodoLista *aux = lista->ini;

    while (aux != NULL) {
        printf("%c", aux->ingrediente);
        aux = aux->prox;
    }
    printf("\n");
}

int inserirFimLista(ListaEnc *lista, char ingrediente) {
    if (lista == NULL)
        return -1;

    NodoLista *novo = (NodoLista*) malloc(sizeof(NodoLista));
    if (novo == NULL)
        return 0; //

    novo->ingrediente = ingrediente;
    novo->prox = NULL;

    if (lista->fim == NULL)  
        lista->ini = novo;
    else
        lista->fim->prox = novo; 

    lista->fim = novo;
    return 1;
}

int montaReceita(ListaEnc *lista, const char *simbolos) {
    if (lista == NULL || simbolos == NULL)
        return 0;

    inicializaLista(lista);

    for (int i = 0; simbolos[i] != '\0'; i++) {
        if (inserirFimLista(lista, simbolos[i]) != 1) {
            destroiLista(lista); 
            return 0;
        }
    }

    return 1;
}

void destroiLista(ListaEnc *lista) {
    if (lista == NULL)
        return;

    NodoLista *aux = lista->ini;
    NodoLista *ant;

    while (aux != NULL) {
        ant = aux;
        aux = aux->prox;
        free(ant);
    }

    lista->ini = NULL;
    lista->fim = NULL;
}