#include <stdlib.h>
#include "hash_tad.h"

// calcula o indice da chave na tabela utilizando o metodo da divisao
// retorna -1 se o tamanho da tabela for invalido
int hashDivisao(int chave, int m) {
    int indice;

    // verifica se o tamanho da tabela é válido
    // não podemos calcular o resto de uma divisão por zero
    if (m <= 0)
        return -1;

    // calcula a posição da chave na tabela hash
    indice = chave % m;

    // se o resultado for negativo, ajusta para uma posição válida para que o índice sempre fique entre 0 e m - 1.
    if (indice < 0)
        indice += m;

    return indice;
}

// inicializa a tabela hash vazia e zera os contadores
// retorna 1 em caso de sucesso e 0 em caso de erro
int hashInicializar(TabelaHash *t, int m){
    if (t == NULL || m <= 0)
        return 0;
    t->tab = (NoHash **) calloc((size_t)m, sizeof(NoHash *)); // aloca o vetor e inicializa os ponteiros das listas com NULL
    if (t->tab == NULL)
        return 0;
    t->m = m;
    t->n = 0;
    t->hits = 0;
    t->misses = 0;
    t->colisoes = 0;
    return 1;


}

// busca um pedido pelo codigo na tabela hash e atualiza os contadores de hits e misses
// retorna 1 se encontrado e 0 caso contrario ou em caso de erro
int hashBuscar(TabelaHash *t, int cod, Pedido *out) {
    // verifica se a tabela e o ponteiro de saida sao validos
    if (t == NULL || t->tab == NULL || t->m <= 0 || out == NULL)
        return 0;

    // calcula a posicao do pedido na tabela hash
    int i = hashDivisao(cod, t->m);

    // comeca a busca pela lista encadeada dessa posicao
    NoHash *p = t->tab[i];

    while (p != NULL) {
        if (p->dado.cod == cod) {
            *out = p->dado;
            t->hits++;
            return 1;
        }
        p = p->prox;
    }
    t->misses++;
    return 0;
}

// insere um pedido na tabela hash utilizando encadeamento separado
// nao permite a insercao de pedidos com codigos duplicados
// retorna 1 em caso de sucesso ou se o pedido ja existe e 0 em caso de erro
int hashInserir(TabelaHash *t, Pedido ped) {
    // verifica se a tabela foi inicializada corretamente
    if (t == NULL || t->tab == NULL || t->m <= 0)
        return 0;

    int i = hashDivisao(ped.cod, t->m);

    NoHash *p = t->tab[i]; // percorre a lista para verificar se o pedido ja esta armazenado
    while (p != NULL) {
        if (p->dado.cod == ped.cod)
            return 1; // ja esta no cache
        p = p->prox;
    }

    NoHash *novo = (NoHash *) malloc(sizeof(NoHash));
    if (novo == NULL)
        return 0;

    if (t->tab[i] != NULL) // posicao ja ocupada: 1 colisao
        t->colisoes++;

    // insere o novo nodo no inicio da lista encadeada
    novo->dado = ped;
    novo->prox = t->tab[i];
    t->tab[i] = novo;
    t->n++;
    return 1;
}

// libera os nodos de todas as listas encadeadas e o vetor da tabela hash
void hashDestruir(TabelaHash *t) {
    if (t == NULL || t->tab == NULL)
        return;
    for (int i = 0; i < t->m; i++) {
        NoHash *p = t->tab[i];
        while (p != NULL) {
            NoHash *prox = p->prox;
            free(p);
            p = prox;
        }
    }
    free(t->tab);
    t->tab = NULL;
    t->m = 0;
    t->n = 0;
}