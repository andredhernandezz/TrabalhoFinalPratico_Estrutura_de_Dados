#ifndef CARREGAMENTO_H
#define CARREGAMENTO_H

#include "avl_tad.h"

// carrega os pedidos do arquivo na AVL
// retorna a quantidade de pedidos lidos em caso de sucesso
// retorna -1 para parametros invalidos ou falha na abertura do arquivo
// retorna -2 para erro de leitura ou dados invalidos
// retorna -3 para falha de alocacao de memoria
int carregarPedidos(const char *nomeArquivo, NodoAvl **raiz);

#endif