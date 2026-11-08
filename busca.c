#include "busca.h"
#include "timer.h"
 // aqui o codigo de busca esta separado do hash.c e hash.h por motivos de organizacao de codigo, fazendo com que o hash.h nao inclua avl e somente o busca.h.

int buscarPedido(NodoAvl *raiz, TabelaHash *cache, int cod,
                 Pedido *out, MetricasBusca *mb) {
    if (hashBuscar(cache, cod, out))
        return 1; // avl nao consulta (hit)
 
    int antes = avlObterMetricas().comparacoes;
 
    Timer t;
    timer_iniciar(&t);
    NodoAvl *no = avlBuscar(raiz, cod);
    double s = timer_parar(&t);
 
    mb->buscas++;
    mb->comparacoes += avlObterMetricas().comparacoes - antes;
    mb->tempo += s;
 
    if (no == NULL)
        return 0;
 
    *out = no->dado;
    hashInserir(cache, no->dado);
    return 1;
}