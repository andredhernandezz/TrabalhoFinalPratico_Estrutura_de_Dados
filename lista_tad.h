#ifndef LISTA_TAD_H
#define LISTA_TAD_H

typedef struct str_NodoLista NodoLista;

// nodo da lista encadeada
struct str_NodoLista {
    char ingrediente; // simbolo do ingrediente
    NodoLista *prox; // ponteiro para o proximo nodo
};

// lista encadeada utilizada para armazenar a receita
typedef struct {
    NodoLista *ini; // ponteiro para o primeiro nodo
    NodoLista *fim; // ponteiro para o ultimo nodo, permite insercao em O(1)
} ListaEnc;

void inicializaLista(ListaEnc *lista); /* cria lista vazia */

int  tamanhoLista(const ListaEnc *lista); /* nº de ingredientes, -1 se lista inválida */

void imprimeLista(const ListaEnc *lista); /* imprime os símbolos em ordem */

int  inserirFimLista(ListaEnc *lista, char ingrediente); /* 1 ok, 0 falha de alocação, -1 lista inválida */

int  montaReceita(ListaEnc *lista, const char *simbolos);/* cria a lista a partir da string; 1 ok, 0 falha */

void destroiLista(ListaEnc *lista); /* libera todos os nodos */

#endif