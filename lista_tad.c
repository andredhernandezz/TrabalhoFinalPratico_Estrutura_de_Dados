#include <stdio.h>
#include <stdlib.h>
#include "lista_tad.h"

// inicializa uma lista vazia, definindo os ponteiros ini e fim como NULL
void inicializaLista(ListaEnc *lista) {
    if (lista == NULL)
        return;

    lista->ini = NULL;
    lista->fim = NULL;
}

// percorre a lista para contar a quantidade de ingredientes em O(n)
// retorna -1 se a lista for invalida
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

// imprime os simbolos dos ingredientes na ordem em que aparecem na lista
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

// insere um novo ingrediente no final da lista em O(1)
// retorna 1 se sucesso, 0 se falha de alocacao e -1 se a lista for invalida
int inserirFimLista(ListaEnc *lista, char ingrediente) {
    if (lista == NULL)
        return -1;

    NodoLista *novo = (NodoLista*) malloc(sizeof(NodoLista));
    if (novo == NULL)
        return 0; // falha na alocacao de memoria 

    novo->ingrediente = ingrediente;
    novo->prox = NULL;

    // se a lista estiver vazia, o novo nodo tambem sera o primeiro
    // caso contrario, conecta o ultimo nodo ao novo
    if (lista->fim == NULL)  
        lista->ini = novo;
    else
        lista->fim->prox = novo; 

    lista->fim = novo;
    return 1;
}

// monta uma lista de ingredientes a partir da string de simbolos da receita
// retorna 1 em caso de sucesso e 0 em caso de falha
int montaReceita(ListaEnc *lista, const char *simbolos) {
    if (lista == NULL || simbolos == NULL)
        return 0;

    inicializaLista(lista);

    // insere cada ingrediente no final da lista, preservando a ordem da receita
    for (int i = 0; simbolos[i] != '\0'; i++) {
        if (inserirFimLista(lista, simbolos[i]) != 1) {
            destroiLista(lista); // libera a lista parcialmente montada em caso de falha
            return 0;
        }
    }

    return 1;
}

// libera todos os nodos da lista e restaura seu estado inicial
void destroiLista(ListaEnc *lista) {
    if (lista == NULL)
        return;

    NodoLista *aux = lista->ini;
    NodoLista *ant;

    // guarda o endereco do proximo nodo antes de liberar o atual
    while (aux != NULL) {
        ant = aux;
        aux = aux->prox;
        free(ant);
    }

    lista->ini = NULL;
    lista->fim = NULL;
}