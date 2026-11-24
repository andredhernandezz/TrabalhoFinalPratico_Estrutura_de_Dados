#include <stdio.h>
#include "metricas.h"

// escreve as metricas do carregamento inicial da AVL e do historico
static void escreveCarga(FILE *out, const MetricasCarga *c) {
    fprintf(out, "-- AVL: carregamento --\n");
    fprintf(out, "Nos inseridos:                   %d\n", c->nos);
    fprintf(out, "Altura resultante:               %d\n", c->altura);
    fprintf(out, "Rotacoes (insercao):             %d\n", c->rotacoes);
    fprintf(out, "Tempo de carregamento:           %.6f s\n", c->tempoCarga);
    fprintf(out, "\n-- Historico: carregamento --\n");
    fprintf(out, "Entradas carregadas:             %d\n", c->entradasHist);
}

// calcula a media de comparacoes por busca e escreve as metricas das buscas na AVL
static void escreveBuscas(FILE *out, const MetricasBusca *b) {
    // evita divisao por zero caso nenhuma busca tenha sido realizada
    double media = (b->buscas > 0) ? (double)b->comparacoes / b->buscas : 0.0;

    fprintf(out, "-- AVL: buscas durante a partida --\n");
    fprintf(out, "Total de buscas:                 %d\n", b->buscas);
    fprintf(out, "Total de comparacoes:            %ld\n", b->comparacoes);
    fprintf(out, "Media de comparacoes por busca:  %.2f\n", media);
    fprintf(out, "Tempo total acumulado:           %.6f s\n", b->tempo);
}

// calcula o fator de carga e a taxa de acerto e escreve as estatisticas da tabela hash
static void escreveHash(FILE *out, const MetricasHash *h) {
    long total = h->hits + h->misses; // total de consultas
    double lambda = (h->m > 0) ? (double)h->n / h->m : 0.0; // fator de carga
    double taxa = (total > 0) ? 100.0 * (double)h->hits / total : 0.0; // percentual de acertos

    fprintf(out, "-- Tabela hash --\n");
    fprintf(out, "Tamanho (m):                     %d\n", h->m);
    fprintf(out, "Entradas ocupadas (n):           %d\n", h->n);
    fprintf(out, "Fator de carga (lambda):         %.2f\n", lambda);
    fprintf(out, "Total de hits:                   %ld\n", h->hits);
    fprintf(out, "Total de misses:                 %ld\n", h->misses);
    fprintf(out, "Taxa de acerto:                  %.1f%%\n", taxa);
    fprintf(out, "Total de colisoes:               %ld\n", h->colisoes);
}

// escreve o algoritmo utilizado, a quantidade de registros e o tempo de ordenacao
static void escreveOrdenacao(FILE *out, const char *alg, int tamanho, double tempo) {
    fprintf(out, "-- Ordenacao do historico (ao acessar Historico) --\n");
    fprintf(out, "Tamanho: %d entradas\n", tamanho);
    fprintf(out, "%s:  %.6f s\n", alg, tempo);
}

// cria ou sobrescreve o arquivo de log com as metricas do carregamento inicial
// retorna 1 se sucesso e 0 se houver erro na gravacao
int metricasLogCarga(const char *arqLog, const MetricasCarga *c) {
    FILE *f = fopen(arqLog, "w");

    // verifica se o arquivo foi aberto
    if (f == NULL)
        return 0;

    // escreve as metricas de carregamento
    escreveCarga(f, c);

    // verifica se ocorreu algum erro durante a escrita
    int erro = ferror(f);

    // fclose tambem pode indicar erro ao finalizar a gravacao
    if (fclose(f) != 0)
        erro = 1;

    // retorna 1 se tudo funcionou, ou 0 se houve erro
    return !erro;
}

// exibe o relatorio de desempenho na tela e o acrescenta ao arquivo de log
// retorna 1 se sucesso na gravacao do arquivo e 0 caso contrario
int metricasRelatorioPartida(const char *arqLog, const MetricasCarga *c,
                             const MetricasBusca *b, const MetricasHash *h) {
    FILE *f = fopen(arqLog, "a");

    // imprime o relatorio na tela e, se possivel, no arquivo
    for (int i = 0; i < 2; i++) {
        FILE *out = (i == 0) ? stdout : f;

        // se nao conseguiu abrir o arquivo, ainda exibe na tela
        if (out == NULL)
            continue;

        fprintf(out, "\n=== RELATORIO DE DESEMPENHO ===\n\n");
        escreveCarga(out, c);
        fprintf(out, "\n");
        escreveBuscas(out, b);
        fprintf(out, "\n");
        escreveHash(out, h);
        fprintf(out, "\n================================\n");
    }

    // nao conseguiu abrir o arquivo para gravacao
    if (f == NULL)
        return 0;

    // verifica se houve erro ao escrever no arquivo
    int erro = ferror(f);

    // verifica tambem se o fechamento foi bem-sucedido
    if (fclose(f) != 0)
        erro = 1;

    return !erro;
}

// exibe as metricas da ordenacao na tela e as acrescenta ao arquivo de log
// retorna 1 se sucesso na gravacao do arquivo e 0 caso contrario
int metricasLogOrdenacao(const char *arqLog, const char *algoritmo,
                         int tamanho, double tempo) {
    FILE *f = fopen(arqLog, "a");

    // exibe as metricas da ordenacao na tela
    escreveOrdenacao(stdout, algoritmo, tamanho, tempo);

    // verifica se o arquivo foi aberto
    if (f == NULL)
        return 0;

    // registra as metricas no arquivo
    fprintf(f, "\n");
    escreveOrdenacao(f, algoritmo, tamanho, tempo);

    // verifica se ocorreu algum erro durante a escrita
    int erro = ferror(f);

    // verifica se o arquivo foi fechado corretamente
    if (fclose(f) != 0)
        erro = 1;

    return !erro;
}