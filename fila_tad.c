#include <stdio.h>
#include <stdlib.h>
#include "fila_tad.h"

void inicializaFila(FilaEnc *f){
    if (f == NULL)
        return; // retorna erro por endereço de fila inválido

    f->frente = NULL;
    f->final = NULL;
}

int filaVazia(const FilaEnc *f){
    if (f == NULL)
        return -1; // retorna erro por fila não inicializada

    return f->frente == NULL;
}

int frenteFila(const FilaEnc *f, Cliente *cliente){
    if (f == NULL || cliente == NULL)
        return -1; // retorna erro por fila não inicializada ou cliente inválido
    if (filaVazia(f))
        return 0; // Fila vazia

    *cliente = f->frente->cliente; // copia o dado do primeiro
    return 1; // deu certo
} 

int enqueue(FilaEnc *f, Cliente cliente){
    
    if (f == NULL)
        return -1;
    Nodo *novo = (Nodo*) malloc(sizeof(Nodo));

    if (novo == NULL)
        return 0;

    novo->cliente = cliente;
    novo->prox = NULL;

    if (f->final != NULL)
        f->final->prox = novo; // o antigo final passa a apontar para o novo nodo
    else
        f->frente = novo;      // fila estava vazia: novo nodo também é a frente

    f->final = novo;
    return 1;
}

int dequeue(FilaEnc *f, Cliente *cliente){
    if (f == NULL || cliente == NULL)
        return -1; // retorna erro por fila não inicializada ou cliente inválido
    
    Nodo *removido;

    if (filaVazia(f)) // underflow
        return 0;

    removido = f->frente;
    *cliente = removido->cliente;
    f->frente = removido->prox;

    if (f->frente == NULL) // a fila ficou vazia
        f->final = NULL;

    free(removido);
    return 1; // deu certo
}

int tamanhoFila(const FilaEnc *f){
    if (f == NULL)
        return -1; // retorna erro por fila não inicializada
    
    Nodo *aux = f->frente;
    int contador = 0;

    while (aux != NULL) { // percorre a fila até o final
        contador++;
        aux = aux->prox;
    }

    return contador;
}

void imprimeFila(const FilaEnc *f){
    
    if (f == NULL) {
        printf("ERRO em imprimeFila: Fila não inicializada.\n");
        return;
    }

    Nodo *aux = f->frente;

    if (filaVazia(f)) {
        printf("Nenhum cliente na fila.\n");
        return;
    }

    printf("Clientes na fila:\n");
    while (aux != NULL) {
        printf("Nome: %s, Pedido: %s, Paciência: %d\n", aux->cliente.nome, aux->cliente.pedido.ingredientes, aux->cliente.paciencia);
        aux = aux->prox;
    }
}

void esvaziaFila(FilaEnc *f){
    if (f == NULL) {
        printf("ERRO em esvaziaFila: Fila não inicializada.\n");
        return;
    }

    Nodo *aux = f->frente;
    Nodo *temp;

    if (filaVazia(f)) {
        return; // a fila já está vazia
    }

    while (aux != NULL) {
        temp = aux;
        aux = aux->prox;
        free(temp);
    }

    f->frente = NULL;
    f->final = NULL;
}