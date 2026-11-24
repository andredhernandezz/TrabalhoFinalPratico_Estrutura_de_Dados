#ifndef METRICAS_H
#define METRICAS_H

// metricas do carregamento inicial
typedef struct {
    int    nos;            /* nos inseridos na AVL            */
    int    altura;         /* altura resultante da AVL        */
    int    rotacoes;       /* rotacoes na insercao            */
    double tempoCarga;     /* tempo total do carregamento (s) */
    int    entradasHist;   /* entradas do historico carregadas */
} MetricasCarga;

// metricas das buscas na AVL
typedef struct {
    int    buscas;         /* quantidade de buscas realizadas */
    long   comparacoes;    /* total de comparacoes realizadas */
    double tempo;          /* tempo total acumulado das buscas (s) */
} MetricasBusca;

// estatisticas da hash
typedef struct {
    int  m;           /* tamanho da tabela             */
    int  n;           /* entradas armazenadas          */
    long hits;        /* consultas com acerto na hash  */
    long misses;      /* consultas sem acerto na hash  */
    long colisoes;    /* total de colisoes registradas */
} MetricasHash;

// cria ou sobrescreve as metricas no medicoes.log
// retorna 1 se sucesso na gravacao e 0 se erro
int metricasLogCarga(const char *arqLog, const MetricasCarga *c);

// Ao encerrar a partida: exibe o Relatorio de Desempenho na tela e o acrescenta ao log.
// retorna 1 se sucesso na gravacao e 0 se erro
int metricasRelatorioPartida(const char *arqLog, const MetricasCarga *c, const MetricasBusca *b, const MetricasHash *h);

// Ao acessar o Historico: exibe e acrescenta ao log o tempo da ordenacao.
// retorna 1 se sucesso na gravacao e 0 se erro
int metricasLogOrdenacao(const char *arqLog, const char *algoritmo, int tamanho, double tempo);

#endif