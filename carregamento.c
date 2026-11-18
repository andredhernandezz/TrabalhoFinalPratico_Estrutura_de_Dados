#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "carregamento.h"

#define MAX_LINHA 256

// verifica se o texto representa um codigo inteiro positivo valido
// retorna 1 se for valido e 0 caso contrario
static int codigoValido(const char *texto, int *codigo) {
    int i = 0;
    int valor = 0;
    int digitos = 0;

    // ignora espacos e tabulacoes antes do numero
    while (texto[i] == ' ' || texto[i] == '\t')
        i++;

    // permite o sinal positivo
    if (texto[i] == '+')
        i++;

    // le os digitos e monta o numero
    while (texto[i] >= '0' && texto[i] <= '9') {
        int digito = texto[i] - '0';

        // verifica se a proxima operacao ultrapassaria INT_MAX
        if (valor > (INT_MAX - digito) / 10)
            return 0;

        valor = valor * 10 + digito;
        digitos++;
        i++;
    }

    // ignora espacos e a quebra de linha depois do numero
    while (texto[i] == ' ' || texto[i] == '\t' ||
           texto[i] == '\n' || texto[i] == '\r')
        i++;

    // rejeita numeros vazios, zero ou caracteres adicionais
    if (digitos == 0 || valor == 0 || texto[i] != '\0')
        return 0;

    *codigo = valor;
    return 1;
}

int carregarPedidos(const char *nomeArquivo, NodoAvl **raiz) {

    // verifica se os parametros sao validos
    if (nomeArquivo == NULL || raiz == NULL)
        return -1;

    FILE *f = fopen(nomeArquivo, "r");

    if (f == NULL)
        return -1;

    Pedido p;
    char linhaCodigo[MAX_LINHA];
    char linhaReceita[MAX_LINHA];
    char extra;
    int lidos = 0;

    // cada pedido ocupa duas linhas, a primeira contem o codigo e a segunda contem a receita
    while (fgets(linhaCodigo, MAX_LINHA, f) != NULL) {

        // verifica se a linha do codigo nao ultrapassou o limite
        if (strchr(linhaCodigo, '\n') == NULL && (int)strlen(linhaCodigo) == MAX_LINHA - 1) {
            fclose(f);
            return -2;
        }

        // todo codigo deve ter uma linha de receita correspondente
        if (fgets(linhaReceita, MAX_LINHA, f) == NULL) {
            fclose(f);
            return -2;
        }

        // verifica se a linha da receita nao ultrapassou o limite.
        if (strchr(linhaReceita, '\n') == NULL && (int)strlen(linhaReceita) == MAX_LINHA - 1) {
            fclose(f);
            return -2;
        }

        // verifica o codigo antes de converter para inteiro
        // rejeita valores negativos, 0, numeros grandes demais e texto extra
        if (!codigoValido(linhaCodigo, &p.cod)) {
            fclose(f);
            return -2;
        }

        // le a receita, respeitando o limite do vetor
        // rejeita receitas com palavras adicionais
        if (sscanf(linhaReceita, "%14s %c", p.ingredientes, &extra) != 1) {
            fclose(f);
            return -2;
        }

        // confere se a receita contem apenas simbolos conhecidos
        int tamanho = (int) strlen(p.ingredientes);

        if (tamanho == 0 || p.ingredientes[tamanho - 1] != 'z') {
            fclose(f);
            return -2;
        }

        for (int i = 0; i < tamanho; i++) {
            if (strchr("kmpasqctnobz", p.ingredientes[i]) == NULL) {
                fclose(f);
                return -2;
            }
        }

        // insere o pedido na arvore avl
        *raiz = avlInserir(*raiz, p);
        lidos++;
    }

    // verifica se a leitura terminou normalmente
    if (ferror(f)) {
        fclose(f);
        return -2;
    }

    fclose(f);
    return lidos;
}
