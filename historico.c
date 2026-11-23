#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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

// verifica se a pontuacao e um inteiro valido (-2147483648 a 2147483647)
static int pontuacaoValida(const char *texto) {
    int i = 0;
    int negativo = 0;
    int digitos = 0;
    long long valor = 0;

    // permite um sinal negativo no inicio.
    if (texto[i] == '-') {
        negativo = 1;
        i++;
    }
    else if (texto[i] == '+') {
        i++;
    }

    // verifica cada caractere da pontuacao.
    while (texto[i] != '\0') {
        if (texto[i] < '0' || texto[i] > '9')
            return 0;

        digitos++;

        // verifica o limite antes de multiplicar por 10, impedindo que a propria verificacao estoure
        if (valor > 2147483648LL / 10)
            return 0;

        valor = valor * 10 + (texto[i] - '0');

        if (valor > 2147483648LL)
            return 0;

        i++;
    }

    // precisa existir pelo menos um digito
    if (digitos == 0)
        return 0;

    // numeros positivos nao podem ultrapassar INT_MAX
    if (!negativo && valor > 2147483647LL)
        return 0;

    return 1;
}

int carregarHistorico(const char *nomeArquivo, ListaContig *lista) {
    // verifica se os parametros e a lista sao validos
    if (nomeArquivo == NULL || lista == NULL || lista->dados == NULL)
        return -1;

    FILE *f = fopen(nomeArquivo, "r");

    // se o arquivo nao existir, considera o historico vazio
    if (f == NULL)
        return 0;

    char linhaNome[MAX_LINHA], linhaPont[MAX_LINHA];
    int carregadas = 0;

    while (lerLinha(f, linhaNome, sizeof linhaNome) &&
           lerLinha(f, linhaPont, sizeof linhaPont)) {

        // ignora registros com pontuacao invalida ou fora do intervalo
        if (!pontuacaoValida(linhaPont))
            continue;

        // converte a pontuacao somente depois de valida-la
        int pont = (int) strtol(linhaPont, NULL, 10);

        NovoHistorico h;
        strncpy(h.nome, linhaNome, sizeof h.nome - 1);
        h.nome[sizeof h.nome - 1] = '\0';
        h.pontuacao = pont;

        // interrompe o carregamento se nao conseguir inserir o registro
        if (insereFimContig(lista, h) != 1) {
            fclose(f);
            return -1;
        }

        carregadas++;
    }

    fclose(f);
    return carregadas;
}

int salvarPartida(const char *nomeArquivo, ListaContig *lista, const char *nome, int pontuacao) {
    // verifica se os parametros recebidos sao validos
    if (nomeArquivo == NULL || lista == NULL || nome == NULL || lista->dados == NULL)
        return 0;

    NovoHistorico h;

    // conta os caracteres ate uma quebra de linha
    int n = (int) strcspn(nome, "\r\n");

    if (n > MAX_NOME) // limita o nome a 20 caracteres
        n = MAX_NOME;

    memcpy(h.nome, nome, n);
    h.nome[n] = '\0';
    h.pontuacao = pontuacao;

    // se a lista estiver cheia, reserva mais memoria antes de tentar gravar a partida no arquivo
    if (lista->tamanho >= lista->capacidade) {

        int novaCapacidade = lista->capacidade * 2;

        NovoHistorico *novo = realloc(
            lista->dados,
            novaCapacidade * sizeof(NovoHistorico)
        );

        // se nao houver memoria, nao grava a partida no arquivo
        if (novo == NULL)
            return 0;

        lista->dados = novo;
        lista->capacidade = novaCapacidade;
    }

    // abre o arquivo para leitura e escrita no final e se nao existir, cria um novo
    FILE *f = fopen(nomeArquivo, "ab+");

    if (f == NULL)
        return 0;

    // posiciona no final para verificar o ultimo caractere
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return 0;
    }

    long tamanho = ftell(f);

    if (tamanho < 0) {
        fclose(f);
        return 0;
    }

    // se o arquivo nao estiver vazio, verifica se termina com uma quebra de linha
    if (tamanho > 0) {

        if (fseek(f, -1, SEEK_END) != 0) {
            fclose(f);
            return 0;
        }

        int ultimo = fgetc(f);

        if (ultimo == EOF) {
            fclose(f);
            return 0;
        }

        // se nao houver quebra de linha, adiciona uma para separar o novo registro do anterior
        if (ultimo != '\n') {
            if (fputc('\n', f) == EOF) {
                fclose(f);
                return 0;
            }
        }
    }

    // grava o nome e a pontuacao em duas linhas
    int ok = fprintf(f, "%s\n%d\n", h.nome, h.pontuacao) > 0;

    // confirma se o arquivo foi fechado corretamente.
    if (fclose(f) != 0)
        ok = 0;

    if (!ok)
        return 0;

    // como a memoria ja foi reservada, da pra inserir diretamente na proxima posicao livre da lista
    lista->dados[lista->tamanho] = h;
    lista->tamanho++;

    return 1;
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