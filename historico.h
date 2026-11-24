#ifndef HISTORICO_H
#define HISTORICO_H

#include "lista_contig_tad.h"

// carrega os registros validos do arquivo na lista contigua
// retorna a quantidade carregada, 0 se nao abrir o arquivo ou -1 em caso de parametros invalidos ou falha de insercao
int carregarHistorico(const char *nomeArquivo, ListaContig *lista);

// salva uma nova partida no arquivo e na lista contigua
// retorna 1 se sucesso e 0 se falha
int salvarPartida(const char *nomeArquivo, ListaContig *lista, const char *nome, int pontuacao);

// exibe uma copia ordenada do historico e informa o tempo de ordenacao
// porPontuacao diferente de 0 ordena por pontuacao, 0 ordena alfabeticamente
// retorna o total de registros ou -1 em caso de falha
int mostrarHistorico(const ListaContig *lista, int porPontuacao, double *tempoOrdenacao);

#endif

 