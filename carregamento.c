#include <stdio.h>
#include "carregamento.h"

int carregarPedidos(const char *nomeArquivo, NodoAvl **raiz) {
    FILE *f = fopen(nomeArquivo, "r");
    if (f == NULL)
        return -1;

    Pedido p;
    int lidos = 0;

    while (fscanf(f, "%d %14s", &p.cod, p.ingredientes) == 2) {
        *raiz = avlInserir(*raiz, p);
        lidos++;
    }

    fclose(f);
    return lidos;
}
