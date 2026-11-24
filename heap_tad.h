
#ifndef HEAP_TAD_H
#define HEAP_TAD_H

#define CAP 1024 // capacidade maxima do heap

typedef struct {
    int dados[CAP]; // vetor que armazena os turnos de vencimento
    int tam; // quantidade de elementos armazenados
} HeapMinimo;

// inicializa um heap vazio
void inicializaHeap(HeapMinimo *heap);

// verifica se o heap esta vazio
// retorna 1 se vazio, 0 se não vazio e -1 se invalido
int heapVazio(const HeapMinimo *heap);

// insere um turno de vencimento
// retorna 1 em caso de sucesso, 0 se cheio e -1 se invalido
int insereHeap(HeapMinimo *heap, int vencimento);

// consulta o menor turno de vencimento, sem remover
// retorna 1 em caso de sucesso, 0 se vazio e -1 se invalido
int consultaMinHeap(const HeapMinimo *heap, int *vencimento);

// remove o menor turno de vencimento
// retorna 1 em caso de sucesso, 0 se vazio e -1 se invalido
int removeMinHeap(HeapMinimo *heap, int *vencimento);

// retorna a quantidade de elementos ou -1 se invalido
int tamanhoHeap(const HeapMinimo *heap);

#endif
