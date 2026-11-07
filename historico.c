#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "historico.h"
#include "ordenacao.h"
#include "timer.h"

#define MAX_LINHA 256
#define MAX_NOME 20
#define LINHAS_HISTORICO 20

/* Le uma linha de f em buf, sem \r/\n. Descarta o excedente de linhas longas.
   Retorna 1 se leu, 0 se chegou ao fim do arquivo. */
static int lerLinha(FILE *f, char *buf, size_t n) {
    if (!fgets(buf, (int)n, f))
        return 0;
    if (!strchr(buf, '\n') && !feof(f)) {
        int c;
        while ((c = fgetc(f)) != '\n' && c != EOF) {}
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

int carregarHistorico(const char *nomeArquivo, ListaContig *lista) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f)
        return 0;

    char linhaNome[MAX_LINHA], linhaPont[MAX_LINHA];
    int carregadas = 0;

    while (lerLinha(f, linhaNome, sizeof linhaNome) &&
           lerLinha(f, linhaPont, sizeof linhaPont)) {
        char *fim;
        long pont = strtol(linhaPont, &fim, 10);
        if (fim == linhaPont || *fim != '\0')
            continue;                       /* pontuacao invalida: ignora o registro */

        NovoHistorico h;
        strncpy(h.nome, linhaNome, sizeof h.nome - 1);
        h.nome[sizeof h.nome - 1] = '\0';
        h.pontuacao = (int)pont;

        if (insereFimContig(lista, h) != 1)
            break;                          /* sem memoria */
        carregadas++;
    }

    fclose(f);
    return carregadas;
}

int salvarPartida(const char *nomeArquivo, ListaContig *lista, const char *nome, int pontuacao) {
    if (!nomeArquivo || !lista || !nome)
        return 0;
 
    NovoHistorico h;
    size_t n = strcspn(nome, "\r\n");        /* corta no primeiro \r ou \n */
    if (n > MAX_NOME)
        n = MAX_NOME;
    memcpy(h.nome, nome, n);
    h.nome[n] = '\0';
    h.pontuacao = pontuacao;
 
    FILE *f = fopen(nomeArquivo, "a");        /* append: nao apaga o que ja existe */
    if (!f)
        return 0;
    int ok = fprintf(f, "%s\n%d\n", h.nome, h.pontuacao) > 0;
    if (fclose(f) != 0)
        ok = 0;
    if (!ok)
        return 0;
 
    return insereFimContig(lista, h) == 1;
}
 
int mostrarHistorico(const ListaContig *lista, int porPontuacao, double *tempoOrdenacao) {
    ListaContig copia;
    if (copiaListaContig(lista, &copia) != 1)
        return -1;
 
    Timer t;
    timer_iniciar(&t);
    int ok = porPontuacao ? quicksortPontuacao(&copia)
                          : quicksortAlfabetico(&copia);
    double s = timer_parar(&t);
 
    if (ok != 1) {
        destroiListaContig(&copia);
        return -1;
    }
    if (tempoOrdenacao)
        *tempoOrdenacao = s;
 
    int total = tamanhoListaContig(&copia);
    printf("---- HISTORICO (%s) ----\n",
           porPontuacao ? "pontuacao decrescente" : "ordem alfabetica");
    if (total == 0)
        printf("Nenhuma partida registrada.\n");
 
    for (int i = 0; i < total && i < LINHAS_HISTORICO; i++) {
        NovoHistorico h;
        obtemContig(&copia, i, &h);
        printf("%3d. %-20s %d\n", i + 1, h.nome, h.pontuacao);
    }
    if (total > LINHAS_HISTORICO)
        printf("... e mais %d entradas\n", total - LINHAS_HISTORICO);
 
    destroiListaContig(&copia);
    return total;
}