#ifndef BUSCA_H
#define BUSCA_H
 
#include "avl_tad.h"
#include "hash_tad.h"
#include "metricas.h"


int buscarPedido(NodoAvl *raiz, TabelaHash *cache, int cod, Pedido *out, MetricasBusca *mb);
// busca primeiro na hash (cache): hit: devolve sem consultar AVL, miss: busca na AVL e, se achar, insere na hash. retorna 1 se achou, 0 se o codigo nao existe



#endif
 