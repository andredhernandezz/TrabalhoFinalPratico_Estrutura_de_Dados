#ifndef AVL_TAD_H
#define AVL_TAD_H
#include "pedido.h"

typedef struct str_NodoAvl NodoAvl;

struct str_NodoAvl {
    Pedido  dado;
    NodoAvl *esq;
    NodoAvl *dir;
    int altura;
};

typedef struct {
    int rotacoes;
    int nosInserido;
    int comparacoes;
} Contador;


void avlZerarMetricas(void);
Contador avlObterMetricas(void);


int avlAltura(const NodoAvl *no);
int avlFb(const NodoAvl *no);
void avlAtualizarAltura(NodoAvl *no);
NodoAvl* avlRotDir(NodoAvl *y);
NodoAvl* avlRotEsq(NodoAvl *x);
NodoAvl* avlReequilibrar(NodoAvl *raiz);
NodoAvl* avlBuscar(NodoAvl *raiz, int cod);
NodoAvl* avlInserir(NodoAvl *raiz, Pedido valor);
NodoAvl* avlDestruir(NodoAvl *raiz);


#endif