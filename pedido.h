#ifndef PEDIDO_H
#define PEDIDO_H

// estrutura utilizada para representar um pedido
typedef struct {
    char ingredientes[15]; // sequencia de simbolos dos ingredientes da receita
    int cod; // codigo de identificacao do pedido
} Pedido;

#endif