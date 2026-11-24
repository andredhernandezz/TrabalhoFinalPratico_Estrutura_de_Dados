#ifndef BUSCA_H
#define BUSCA_H
 
#include "avl_tad.h"
#include "hash_tad.h"
#include "metricas.h"


// busca primeiro na hash e, em caso de miss, busca na AVL
// se encontrado na AVL, insere o pedido na hash e atualiza as metricas
// retorna 1 se encontrado e 0 se o codigo nao existe
int buscarPedido(NodoAvl *raiz, TabelaHash *cache, int cod, Pedido *out, MetricasBusca *mb);



#endif
 