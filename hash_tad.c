#include <stdlib.h>
#include "hash_tad.h"

int hashDivisao(int chave, int m) {
    return ((chave % m) % m);
}

int hashInicializar(TabelaHash *t, int m){
    if (t == NULL || m <= 0)
        return 0;
    t->tab = (NoHash **) calloc((size_t)m, sizeof(NoHash *)); // calloc pra deixar 0 tudo o que for lixo de memoria
    if (t->tab == NULL)
        return 0;
    t->m = m;
    t->n = 0;
    t->hits = 0;
    t->misses = 0;
    t->colisoes = 0;
    return 1;


}
int hashBuscar(TabelaHash *t, int cod, Pedido *out) {
    NoHash *p = t->tab[hashDivisao(cod, t->m)];
    while (p != NULL) {
        if (p->dado.cod == cod) {
            *out = p->dado;
            t->hits++;
            return 1;
        }
        p = p->prox;
    }
    t->misses++;
    return 0;
}
 
int hashInserir(TabelaHash *t, Pedido ped) {
    int i = hashDivisao(ped.cod, t->m);

    NoHash *p = t->tab[i];
    while (p != NULL) {
        if (p->dado.cod == ped.cod)
            return 1; // ja esta no cache
        p = p->prox;
    }

    NoHash *novo = (NoHash *) malloc(sizeof(NoHash));
    if (novo == NULL)
        return 0;

    if (t->tab[i] != NULL) // posicao ja ocupada: 1 colisao
        t->colisoes++;

    novo->dado = ped;
    novo->prox = t->tab[i];
    t->tab[i] = novo;
    t->n++;
    return 1;
}
 
void hashDestruir(TabelaHash *t) {
    if (t == NULL || t->tab == NULL)
        return;
    for (int i = 0; i < t->m; i++) {
        NoHash *p = t->tab[i];
        while (p != NULL) {
            NoHash *prox = p->prox;
            free(p);
            p = prox;
        }
    }
    free(t->tab);
    t->tab = NULL;
    t->m = 0;
    t->n = 0;
}