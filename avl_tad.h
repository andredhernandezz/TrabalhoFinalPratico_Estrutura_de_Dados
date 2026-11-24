#ifndef AVL_TAD_H
#define AVL_TAD_H
#include "pedido.h"

typedef struct str_NodoAvl NodoAvl;

struct str_NodoAvl {
    Pedido dado;   // pedido armazenado no nodo
    NodoAvl *esq;  // filho esquerdo
    NodoAvl *dir;  // filho direito
    int altura;    // altura da subarvore
};

// estrutura utilizada para armazenar as metricas de desempenho da AVL
typedef struct {
    int rotacoes;     // quantidade de rotacoes realizadas
    int nosInserido;  // quantidade de nodos inseridos
    int comparacoes;  // quantidade de nodos visitados nas buscas
} Contador;


// zera os contadores de desempenho
void avlZerarMetricas(void);

// retorna as metricas acumuladas
Contador avlObterMetricas(void);

// retorna a altura do nodo
int avlAltura(const NodoAvl *no);

// retorna o fator de balanceamento do nodo
int avlFb(const NodoAvl *no);

// informa se a ultima insercao falhou por falta de memoria
int avlErroInsercao(void);

// recalcula a altura do nodo com base nas alturas dos filhos
void avlAtualizarAltura(NodoAvl *no);

// realiza uma rotacao simples a direita e retorna a nova raiz
NodoAvl* avlRotDir(NodoAvl *y);

// realiza uma rotacao simples a esquerda e retorna a nova raiz
NodoAvl* avlRotEsq(NodoAvl *x);

// corrige possiveis desequilibrios e retorna a raiz da subarvore
NodoAvl* avlReequilibrar(NodoAvl *raiz);

// busca um pedido pelo codigo e retorna NULL se nao encontrado
NodoAvl* avlBuscar(NodoAvl *raiz, int cod);

// insere um pedido na AVL e retorna a raiz resultante
// pedidos com codigo duplicado nao sao inseridos
NodoAvl* avlInserir(NodoAvl *raiz, Pedido valor);

// libera todos os nodos da arvore e retorna NULL
NodoAvl* avlDestruir(NodoAvl *raiz);


#endif