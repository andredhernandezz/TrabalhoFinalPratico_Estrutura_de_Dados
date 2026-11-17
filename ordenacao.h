#ifndef ORDENACAO_H
#define ORDENACAO_H
#include "lista_contig_tad.h"

int quicksortAlfabetico(ListaContig *lista); // retorna 1 se ordenou, -1 se lista inválida

int quicksortPontuacao(ListaContig *lista); // retorna 1 se ordenou, -1 se lista inválida

int mergesortAlfabetico(ListaContig *lista); // retorna 1 se ordenou, 0 se falhou o malloc, -1 se lista inválida

int mergesortPontuacao(ListaContig *lista); // retorna 1 se ordenou, 0 se falhou o malloc, -1 se lista inválida

// ordena uma copia de base com alg, mede o tempo, imprime e grava em medicoes.log
void testarOrdenacao(const char *nome, int (*alg)(ListaContig *), const ListaContig *base);

#endif