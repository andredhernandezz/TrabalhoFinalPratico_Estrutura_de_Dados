#ifndef LISTA_TAD_H
#define LISTA_TAD_H

typedef struct str_NodoLista NodoLista;

struct str_NodoLista {
    char ingrediente; /* símbolo do ingrediente  */
    NodoLista *prox;
};

typedef struct {
    NodoLista *ini;
    NodoLista *fim; /* permite inserir no fim */
} ListaEnc;

void inicializaLista(ListaEnc *lista); /* cria lista vazia */
int  tamanhoLista(const ListaEnc *lista); /* nº de ingredientes, -1 se lista inválida */
void imprimeLista(const ListaEnc *lista); /* imprime os símbolos em ordem */
int  inserirFimLista(ListaEnc *lista, char ingrediente); /* 1 ok, 0 falha de alocação, -1 lista inválida */
int  montaReceita(ListaEnc *lista, const char *simbolos);/* cria a lista a partir da string; 1 ok, 0 falha */
void destroiLista(ListaEnc *lista); /* libera todos os nodos */

#endif