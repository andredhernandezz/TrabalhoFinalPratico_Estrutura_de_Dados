#ifndef FILA_TAD_H
#define FILA_TAD_H
#include "pedido.h"

typedef struct {
    char nome[50];
    Pedido pedido;
    int paciencia;
} Cliente;

typedef struct str_Nodo Nodo;

struct str_Nodo {
    Nodo *prox;
    Cliente cliente;
};

typedef struct {
    Nodo *frente;
    Nodo *final;
} FilaEnc;

void inicializaFila(FilaEnc *f); // inicializa uma fila vazia
int filaVazia(const FilaEnc *f); // verifica se a fila está vazia
    // retorna 1 se vazia, 0 se não vazia, -1 se fila não inicializada
int frenteFila(const FilaEnc *f, Cliente *cliente); // consulta dados do primeiro cliente da fila
    // retorna 1 se deu certo, 0 se fila vazia, -1 se fila não inicializada ou cliente inválido
int enqueue(FilaEnc *f, Cliente cliente); // insere um cliente no final da fila
    // retorna 1 se deu certo, 0 se falha na alocação de memória, -1 se fila não inicializada ou falha na alocação de memória
int dequeue(FilaEnc *f, Cliente *cliente); // remove o cliente do início da fila e retorna seus dados
    // retorna 1 se deu certo, 0 se fila vazia, -1 se fila não inicializada ou cliente inválido
int tamanhoFila(const FilaEnc *f);
    // retorna o número de clientes da fila ou -1 se fila não inicializada
void imprimeFila(const FilaEnc *f); // imprime os clientes na fila
void esvaziaFila(FilaEnc *f); // esvazia a fila, liberando memória

#endif