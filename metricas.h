#ifndef METRICAS_H
#define METRICAS_H

/* Metricas obtidas no carregamento inicial (spec 5.1). */
typedef struct {
    int    nos;            /* nos inseridos na AVL            */
    int    altura;         /* altura resultante da AVL        */
    int    rotacoes;       /* rotacoes na insercao            */
    double tempoCarga;     /* tempo total do carregamento (s) */
    int    entradasHist;   /* entradas do historico carregadas */
} MetricasCarga;

/* Metricas das buscas na AVL acumuladas durante a partida (spec 5.2). */
typedef struct {
    int    buscas;
    long   comparacoes;
    double tempo;          /* tempo total acumulado das buscas (s) */
} MetricasBusca;

/* Cria (ou sobrescreve) o log com as metricas de carregamento. */
int metricasLogCarga(const char *arqLog, const MetricasCarga *c);

/* Ao encerrar a partida: exibe o Relatorio de Desempenho na tela e o
   acrescenta ao log. */
int metricasRelatorioPartida(const char *arqLog, const MetricasCarga *c, const MetricasBusca *b);

/* Ao acessar o Historico: exibe e acrescenta ao log o tempo da ordenacao. */
int metricasLogOrdenacao(const char *arqLog, const char *algoritmo, int tamanho, double tempo);

#endif