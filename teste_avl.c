#include <stdio.h>
#include "avl_tad.h"

int main(void) {
    NodoAvl *raiz = NULL;
    Pedido p;

    printf("vazia: %d (esperado 0)\n", avlAltura(raiz));

    p.cod = 1001;
    raiz = avlInserir(raiz, p);
    printf("1 no: %d (esperado 1)\n", avlAltura(raiz));

    p.cod = 1042;
    raiz = avlInserir(raiz, p);
    p.cod = 2017;
    raiz = avlInserir(raiz, p);
    printf("apendice B: %d (esperado 2)\n", avlAltura(raiz));

    raiz = avlDestruir(raiz);
    return 0;
}