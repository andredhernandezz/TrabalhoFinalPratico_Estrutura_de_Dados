#ifndef ORDENACAO_H
#define ORDENACAO_H
#include "lista_contig_tad.h"

// ordena alfabeticamente, usando a maior pontuacao como desempate
int quicksortAlfabetico(ListaContig *lista); // retorna 1 se ordenou, -1 se lista invalida

// ordena por pontuacao decrescente, usando o nome como desempate
int quicksortPontuacao(ListaContig *lista); // retorna 1 se ordenou, -1 se lista invalida

// ordena alfabeticamente utilizando um vetor auxiliar
int mergesortAlfabetico(ListaContig *lista); // retorna 1 se ordenou, 0 se falhou o malloc, -1 se lista invalida

// ordena por pontuacao decrescente utilizando um vetor auxiliar
int mergesortPontuacao(ListaContig *lista); // retorna 1 se ordenou, 0 se falhou o malloc, -1 se lista invalida

// ordena uma copia de base com alg, mede o tempo e grava em medicoes.log
void testarOrdenacao(const char *nome, int (*alg)(ListaContig *), const ListaContig *base);

#endif