#ifndef HISTORICO_H
#define HISTORICO_H

#include "lista_contig_tad.h"

int carregarHistorico(const char *nomeArquivo, ListaContig *lista);

int salvarPartida(const char *nomeArquivo, ListaContig *lista, const char *nome, int pontuacao);

int mostrarHistorico(const ListaContig *lista, int porPontuacao, double *tempoOrdenacao);

#endif

 