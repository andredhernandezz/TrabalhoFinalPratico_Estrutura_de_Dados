#ifndef HASH_TAD_H
#define HASH_TAD_H
 
#include "pedido.h"


#define HASH_M_PADRAO 101 // numero primo 101 (escolha logica)
 
// No da lista encadeada de uma posicao da tabela (encadeamento separado)
typedef struct str_NoHash NoHash;
struct str_NoHash {
    Pedido  dado;
    NoHash *prox;
};
 
typedef struct {
    NoHash **tab; // vetor de m listas encadeadas
    int      m; // tamanho da tabela hash
    int      n; // entradas armazenadas (p/ fator de carga n/m)
    int     hits; // buscas encontradas no cache
    int     misses; // buscas nao encontradas no cache
    int     colisoes; // insercoes que cairam em posicao ocupada
} TabelaHash;
 
// Funcao de hash por divisao: h(chave) = chave mod m, sempre em [0, m-1]
int hashDivisao(int chave, int m);
 
// Cria a tabela vazia com m posicoes e zera os contadores. Retorna 1 se ok, 0 em caso de erro
int hashInicializar(TabelaHash *t, int m);
 
// Busca pelo codigo. Conta um hit (retorna 1 e copia o pedido para *out) ou um miss (retorna 0).
int hashBuscar(TabelaHash *t, int cod, Pedido *out);
 
/* Insere o pedido no inicio da lista da posicao h(cod).
   Conta uma colisao se a posicao ja tinha alguma entrada.
   Se o codigo ja existe, nao duplica. Retorna 1 se ok, 0 se faltou memoria. */
int hashInserir(TabelaHash *t, Pedido p);
 
// Libera toda a memoria da tabela.
void hashDestruir(TabelaHash *t);
 
#endif