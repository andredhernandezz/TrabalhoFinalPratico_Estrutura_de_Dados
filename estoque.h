#ifndef ESTOQUE_H
#define ESTOQUE_H

#include "heap_tad.h"

// validade em turnos de cada tipo de alimento perecivel
#define VENC_CARNE 4
#define VENC_OVO 4
#define VENC_BACON 4

typedef enum {
    CARNE,
    OVO,
    BACON,
    QUEIJO_SUICO,
    QUEIJO_CHEDDAR,
    CEBOLA,
    TOMATE
} IngredienteEstoque; // enum para representar os tipos de ingredientes no estoque

typedef struct {
    // heaps que armazenam os turnos de vencimento dos pereciveis
    HeapMinimo carne;
    HeapMinimo ovo;
    HeapMinimo bacon;
    // quantidade de unidades preparadas dos nao pereciveis
    int queijoSuico;
    int queijoCheddar;
    int cebola;
    int tomate;
} Estoque; // estrutura de dados para representar o estoque de ingredientes que precisam de preparo

// zera os contadores e inicializa os tres heaps de ingredientes pereciveis
void inicializaEstoque(Estoque *estoque);

// prepara uma unidade; para perecivel, insere turnoAtual + validade no heap
// retorna 1 se sucesso, 0 se heap cheio, -1 se estoque ou ingrediente invalido
int preparaIngrediente(Estoque *estoque, IngredienteEstoque ing, int turnoAtual);

// consome uma unidade; para perecivel, remove a que vence primeiro
// retorna 1 se sucesso, 0 se nao ha unidade, -1 se invalido
int consomeIngrediente(Estoque *estoque, IngredienteEstoque ing);

// descarta os perecíveis com vencimento <= turnoAtual
// retorna quantas unidades foram descartadas, ou -1 se invalido
int verificaVencimentos(Estoque *estoque, int turnoAtual);

// consulta a quantidade de unidades disponiveis do ingrediente
// retorna -1 se os parametros forem invalidos
int consultaEstoque(const Estoque *estoque, IngredienteEstoque ing);

// consulta quantos turnos faltam para o vencimento mais proximo
// retorna os turnos restantes, -1 se nao ha ingrediente ou -2 se os parametros forem invalidos
int consultaValidade(const Estoque *estoque, IngredienteEstoque ing, int turnoAtual);

#endif



