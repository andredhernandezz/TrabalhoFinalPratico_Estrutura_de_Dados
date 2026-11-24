#ifndef PILHA_TAD_H
#define PILHA_TAD_H

#include "lista_tad.h"

typedef struct str_NodoPilha NodoPilha;

// nodo da pilha encadeada
struct str_NodoPilha {
    char ingrediente; // simbolo do ingrediente armazenado
    NodoPilha *prox; // ponteiro para o nodo abaixo
};

// estrutura utilizada para representar o hamburguer em montagem
typedef struct {
    NodoPilha *topo; // ponteiro para o ingrediente do topo
    int tamanho; // quantidade de ingredientes empilhados
} PilhaEnc;

void inicializaPilha(PilhaEnc *p); // inicializa uma pilha vazia

int  pilhaVazia(const PilhaEnc *p); // verifica se a pilha está vazia
    // retorna 1 se vazia, 0 se não vazia, -1 se pilha não inicializada

int  topoPilha(const PilhaEnc *p, char *ingrediente); // consulta o topo sem remover
    // retorna 1 se deu certo, 0 se pilha vazia, -1 se pilha não inicializada ou ponteiro inválido

int  push(PilhaEnc *p, char ingrediente); // empilha um ingrediente no topo
    // retorna 1 se deu certo, 0 se falha na alocação, -1 se pilha não inicializada

int  pop(PilhaEnc *p, char *ingrediente); // desempilha o topo e devolve o ingrediente
    // retorna 1 se deu certo, 0 se pilha vazia (underflow), -1 se pilha não inicializada ou ponteiro inválido

int  tamanhoPilha(const PilhaEnc *p);
    // retorna o número de ingredientes ou -1 se pilha não inicializada

void imprimePilha(const PilhaEnc *p); // imprime o hambúrguer de baixo para cima

int  pilhaConfereReceita(const PilhaEnc *p, const ListaEnc *receita);
    // compara a pilha (de baixo para cima) com a receita, sem alterar a pilha
    // retorna 1 se iguais, 0 se diferentes, -1 se argumento inválido
    
void esvaziaPilha(PilhaEnc *p); // esvazia a pilha, liberando memória



#endif