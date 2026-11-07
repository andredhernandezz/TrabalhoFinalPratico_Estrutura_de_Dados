#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "historico.h"

#define MAX_LINHA 256

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