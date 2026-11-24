#include <stdio.h>
#include <stdlib.h>
#include "pilha_tad.h"

// inicializa uma pilha vazia, definindo o topo como NULL e zerando o tamanho
void inicializaPilha(PilhaEnc *p) {
    if (p == NULL)
        return;

    p->topo = NULL;
    p->tamanho = 0;
}

// verifica se a pilha esta vazia
// retorna 1 se vazia, 0 se nao vazia e -1 se a pilha for invalida
int pilhaVazia(const PilhaEnc *p) {
    if (p == NULL)
        return -1;

    return p->topo == NULL;
}

// consulta o ingrediente do topo sem remove-lo da pilha
// retorna 1 se sucesso, 0 se vazia e -1 se os parametros forem invalidos
int topoPilha(const PilhaEnc *p, char *ingrediente) {
    if (p == NULL || ingrediente == NULL)
        return -1; 
    if (pilhaVazia(p))
        return 0;

    *ingrediente = p->topo->ingrediente;
    return 1;
}

// insere um novo ingrediente no topo da pilha em O(1)
// retorna 1 se sucesso, 0 se falha de alocacao e -1 se a pilha for invalida
int push(PilhaEnc *p, char ingrediente) {
    if (p == NULL)
        return -1;

    NodoPilha *novo = (NodoPilha*) malloc(sizeof(NodoPilha));
    if (novo == NULL)
        return 0; 

    // conecta o novo nodo ao topo anterior e atualiza o tamanho da pilha
    novo->ingrediente = ingrediente;
    novo->prox = p->topo;
    p->topo = novo;
    p->tamanho++;
    return 1;
}

// remove o ingrediente do topo da pilha em O(1)
// retorna 1 se sucesso, 0 se vazia e -1 se os parametros forem invalidos
int pop(PilhaEnc *p, char *ingrediente) {
    if (p == NULL || ingrediente == NULL)
        return -1;
    if (pilhaVazia(p))
        return 0;

    // guarda o ingrediente removido e atualiza o topo e o tamanho da pilha
    NodoPilha *removido = p->topo;
    *ingrediente = removido->ingrediente;
    p->topo = removido->prox;
    p->tamanho--;

    free(removido);
    return 1;
}

// retorna a quantidade de ingredientes em O(1) ou -1 se a pilha for invalida
int tamanhoPilha(const PilhaEnc *p) {
    if (p == NULL)
        return -1;

    return p->tamanho;
}

// funcao auxiliar que percorre recursivamente a pilha ate a base
// imprime os ingredientes durante o retorno da recursao
void imprimeRec(const NodoPilha *no) {
    if (no == NULL)
        return;
    imprimeRec(no->prox);
    printf("%c", no->ingrediente);
}

// imprime os ingredientes do hamburguer de baixo para cima sem alterar a pilha
void imprimePilha(const PilhaEnc *p) {
    if (p == NULL) {
        printf("ERRO em imprimePilha: pilha não inicializada.\n");
        return;
    }

    if (pilhaVazia(p)) {
        printf("(prato vazio)\n");
        return;
    }

    imprimeRec(p->topo);
    printf("\n");
}

// compara recursivamente os ingredientes da pilha com os da lista da receita
// realiza a comparacao de baixo para cima, avancando o cursor da lista
// retorna 1 se os ingredientes comparados coincidirem e 0 caso contrario
int confereRec(const NodoPilha *no, const NodoLista **cursor) {
    if (no == NULL)
        return 1; 

    // desce ate a base da pilha antes de iniciar as comparacoes
    if (!confereRec(no->prox, cursor))
        return 0;

    // verifica se ainda existem ingredientes na receita e se os simbolos coincidem
    if (*cursor == NULL) 
        return 0;
    if ((*cursor)->ingrediente != no->ingrediente)
        return 0;

    // avanca para o proximo ingrediente da receita
    *cursor = (*cursor)->prox;
    return 1;
}

// verifica se o hamburguer montado corresponde exatamente a receita
// realiza a comparacao sem alterar a pilha ou a lista
// retorna 1 se iguais, 0 se diferentes e -1 se os parametros forem invalidos
int pilhaConfereReceita(const PilhaEnc *p, const ListaEnc *receita) {
    if (p == NULL || receita == NULL)
        return -1;

    const NodoLista *cursor = receita->ini;

    if (!confereRec(p->topo, &cursor))
        return 0;

    // garante que nao sobraram ingredientes na lista da receita
    return cursor == NULL; 
}

// libera todos os nodos da pilha e restaura seu estado inicial
void esvaziaPilha(PilhaEnc *p) {
    if (p == NULL) {
        printf("ERRO em esvaziaPilha: pilha não inicializada.\n");
        return;
    }

    NodoPilha *aux = p->topo;
    NodoPilha *temp;

    while (aux != NULL) {
        temp = aux;
        aux = aux->prox;
        free(temp);
    }

    p->topo = NULL;
    p->tamanho = 0;
}