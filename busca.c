#include "busca.h"
#include "timer.h"

/*  a busca esta separada da implementacao da hash para evitar
    que hash_tad.h dependa de avl_tad.h
*/

// busca um pedido primeiro na hash e, caso nao encontre, na AVL
// contabiliza as metricas da busca na AVL e armazena o pedido encontrado na hash
// retorna 1 se encontrado e 0 caso contrario
int buscarPedido(NodoAvl *raiz, TabelaHash *cache, int cod,
                 Pedido *out, MetricasBusca *mb) {
    if (hashBuscar(cache, cod, out))
        return 1; // avl nao consulta (hit)

    // registra o contador de comparacoes antes da busca na AVL
    int antes = avlObterMetricas().comparacoes;
 
    // mede o tempo gasto na busca pela AVL
    Timer t;
    timer_iniciar(&t);
    NodoAvl *no = avlBuscar(raiz, cod);
    double s = timer_parar(&t);
 
    // acumula as metricas da busca realizada
    mb->buscas++;
    mb->comparacoes += avlObterMetricas().comparacoes - antes;
    mb->tempo += s;
 
    if (no == NULL)
        return 0;
 
    // retorna o pedido encontrado e armazena uma copia na hash para futuras buscas
    *out = no->dado;
    hashInserir(cache, no->dado);
    return 1;
}