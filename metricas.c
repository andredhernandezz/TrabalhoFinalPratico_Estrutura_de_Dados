#include <stdio.h>
#include "metricas.h"

static void escreveCarga(FILE *out, const MetricasCarga *c) {
    fprintf(out, "-- AVL: carregamento --\n");
    fprintf(out, "Nos inseridos:                   %d\n", c->nos);
    fprintf(out, "Altura resultante:               %d\n", c->altura);
    fprintf(out, "Rotacoes (insercao):             %d\n", c->rotacoes);
    fprintf(out, "Tempo de carregamento:           %.6f s\n", c->tempoCarga);
    fprintf(out, "\n-- Historico: carregamento --\n");
    fprintf(out, "Entradas carregadas:             %d\n", c->entradasHist);
}

static void escreveBuscas(FILE *out, const MetricasBusca *b) {
    double media = (b->buscas > 0) ? (double)b->comparacoes / b->buscas : 0.0;

    fprintf(out, "-- AVL: buscas durante a partida --\n");
    fprintf(out, "Total de buscas:                 %d\n", b->buscas);
    fprintf(out, "Total de comparacoes:            %ld\n", b->comparacoes);
    fprintf(out, "Media de comparacoes por busca:  %.2f\n", media);
    fprintf(out, "Tempo total acumulado:           %.6f s\n", b->tempo);
}

static void escreveHash(FILE *out, const MetricasHash *h) {
    long total = h->hits + h->misses;
    double lambda = (h->m > 0) ? (double)h->n / h->m : 0.0;
    double taxa = (total > 0) ? 100.0 * (double)h->hits / total : 0.0;

    fprintf(out, "-- Tabela hash --\n");
    fprintf(out, "Tamanho (m):                     %d\n", h->m);
    fprintf(out, "Entradas ocupadas (n):           %d\n", h->n);
    fprintf(out, "Fator de carga (lambda):         %.2f\n", lambda);
    fprintf(out, "Total de hits:                   %ld\n", h->hits);
    fprintf(out, "Total de misses:                 %ld\n", h->misses);
    fprintf(out, "Taxa de acerto:                  %.1f%%\n", taxa);
    fprintf(out, "Total de colisoes:               %ld\n", h->colisoes);
}

static void escreveOrdenacao(FILE *out, const char *alg, int tamanho, double tempo) {
    fprintf(out, "-- Ordenacao do historico (ao acessar Historico) --\n");
    fprintf(out, "Tamanho: %d entradas\n", tamanho);
    fprintf(out, "%s:  %.6f s\n", alg, tempo);
}

int metricasLogCarga(const char *arqLog, const MetricasCarga *c) {
    FILE *f = fopen(arqLog, "w");
    if (!f)
        return 0;
    escreveCarga(f, c);
    fclose(f);
    return 1;
}

int metricasRelatorioPartida(const char *arqLog, const MetricasCarga *c,
                             const MetricasBusca *b, const MetricasHash *h) {
    FILE *f = fopen(arqLog, "a");
    for (int i = 0; i < 2; i++) { // i=0: tela, i=1: arquivo
        FILE *out = (i == 0) ? stdout : f;
        if (!out)
            continue;
        fprintf(out, "\n=== RELATORIO DE DESEMPENHO ===\n\n");
        escreveCarga(out, c);
        fprintf(out, "\n");
        escreveBuscas(out, b);
        fprintf(out, "\n");
        escreveHash(out, h);
        fprintf(out, "\n================================\n");
    }
    if (f)
        fclose(f);
    return f != NULL;
}

int metricasLogOrdenacao(const char *arqLog, const char *algoritmo,
                         int tamanho, double tempo) {
    FILE *f = fopen(arqLog, "a");
    escreveOrdenacao(stdout, algoritmo, tamanho, tempo);
    if (!f)
        return 0;
    fprintf(f, "\n");
    escreveOrdenacao(f, algoritmo, tamanho, tempo);
    fclose(f);
    return 1;
}