#include <stdio.h>
#include <stdlib.h>
#include "fila_tad.h"

// inicializa uma fila vazia, definindo os ponteiros frente e final como NULL
void inicializaFila(FilaEnc *f){
    if (f == NULL)
        return; // encerra a funcao se o endereco da fila for invalido

    f->frente = NULL;
    f->final = NULL;
}

// verifica se a fila esta vazia
// retorna 1 se vazia, 0 se nao vazia e -1 se a fila for invalida
int filaVazia(const FilaEnc *f){
    if (f == NULL)
        return -1; // retorna erro por fila não inicializada

    return f->frente == NULL;
}

// consulta os dados do primeiro cliente sem remove-lo da fila
// retorna 1 se sucesso, 0 se fila vazia e -1 se os parametros forem invalidos
int frenteFila(const FilaEnc *f, Cliente *cliente){
    if (f == NULL || cliente == NULL)
        return -1; // retorna erro por fila não inicializada ou cliente inválido
    if (filaVazia(f))
        return 0; // Fila vazia

    *cliente = f->frente->cliente; // copia o dado do primeiro
    return 1; // deu certo
} 

// insere um novo cliente no final da fila em O(1)
// retorna 1 se sucesso, 0 se falha de alocacao e -1 se a fila for invalida
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

// remove o primeiro cliente da fila e copia seus dados em O(1)
// retorna 1 se sucesso, 0 se fila vazia e -1 se os parametros forem invalidos
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

// percorre a fila para contar a quantidade de clientes em O(n)
// retorna -1 se a fila for invalida
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

// percorre a fila e imprime o nome e o pedido de cada cliente
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
        printf("Nome: %s, Pedido: %s", aux->cliente.nome, aux->cliente.pedido.ingredientes);
        aux = aux->prox;
    }
}

// libera todos os nodos da fila e redefine os ponteiros frente e final
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

// consulta os dados do segundo cliente sem remove-lo da fila
// retorna 1 se sucesso, 0 se nao existir segundo cliente e -1 se os parametros forem invalidos
int segundoFila(const FilaEnc *f, Cliente *cliente) {

    // Verifica se os parametros recebidos sao validos.
    if (f == NULL || cliente == NULL)
        return -1;

    // Para existir um segundo cliente, a frente precisa
    // apontar para um nodo seguinte.
    if (f->frente == NULL || f->frente->prox == NULL)
        return 0;

    // Copia os dados do segundo cliente para a variavel recebida.
    *cliente = f->frente->prox->cliente;

    return 1;
}