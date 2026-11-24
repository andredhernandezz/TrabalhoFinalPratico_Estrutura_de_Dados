#include <stddef.h>
#include "heap_tad.h"

// reorganiza o heap de baixo para cima apos a insercao de um elemento
// troca o elemento com seu pai enquanto for menor que ele
static void siftUp(int dados[], int i){
    while (i > 0){
        int pai = (i - 1) / 2; // calcula o indice do pai

        if (dados[i] < dados[pai]){ // se o elemento atual for menor que o pai, troca os elementos
            int temp = dados[i];
            dados[i] = dados[pai];
            dados[pai] = temp;

            i = pai; // atualiza o indice para o pai e continua subindo na arvore
        }
        else{
            break; // se elemento atual >= pai, a propriedade de heap mínimo ja esta satisfeita
        }
    }
}

// reorganiza o heap de cima para baixo
// troca o elemento com seu menor filho quando necessario
static void heapify(int dados[], int n, int i){
    int menor = i;
    int esq = 2*i + 1;
    int dir = 2*i + 2;

    if (esq < n && dados[esq] < dados[menor]) // se o filho esquerdo for menor que o elemento atual
        menor = esq; // atualiza o menor para o filho esquerdo

    if (dir < n && dados[dir] < dados[menor]) // se o filho direito for menor que o menor encontrado ate agora
        menor = dir; // atualiza o menor para o filho direito

    if (menor != i){ // se o menor nao for o elemento atual
        int temp = dados[i];
        dados[i] = dados[menor];
        dados[menor] = temp; // troca os elementos

        heapify(dados, n, menor); // chama recursivamente para o sub-heap afetado
    }
}

// inicializa um heap vazio
void inicializaHeap(HeapMinimo *heap){
    if (heap == NULL)
        return;
    
    heap->tam=0; // inicializa o tamanho do heap como 0
}

// verifica se o heap esta vazio
// retorna 1 se vazio, 0 se nao vazio e -1 se invalido
int heapVazio(const HeapMinimo *heap){
    if (heap == NULL || heap->tam < 0 || heap->tam > CAP) // verifica se o heap é inválido
        return -1;

    return heap->tam == 0; // retorna 1 se vazio, 0 se não vazio
}

// insere um turno de vencimento e reorganiza o heap utilizando siftUp
// retorna 1 em caso de sucesso, 0 se cheio e -1 se invalido
int insereHeap(HeapMinimo *heap, int vencimento){
    if (heap == NULL || heap->tam < 0 || heap->tam > CAP) // verifica se o heap é inválido
        return -1;
    if (heap->tam == CAP) // verifica se o heap está cheio
        return 0;
    
    heap->dados[heap->tam] = vencimento; // insere o vencimento no final do heap
    siftUp(heap->dados, heap->tam); // reorganiza o heap para manter a propriedade de heap mínimo
    heap->tam++; // incrementa o tamanho do heap

    return 1; // sucesso
}

// consulta o menor turno de vencimento sem remover o elemento
// retorna 1 em caso de sucesso, 0 se vazio e -1 se os parametros forem invalidos
int consultaMinHeap(const HeapMinimo *heap, int *vencimento){
    if (heap == NULL || heap->tam < 0 || heap->tam > CAP || vencimento == NULL) // verifica se heap e vencimento são inválidos
        return -1;
    if (heap->tam == 0) // verifica se o heap está vazio
        return 0;
    
    *vencimento = heap->dados[0]; // consulta o menor vencimento (raiz do heap)
    
    return 1; // sucesso
}

// remove o menor turno de vencimento e reorganiza o heap utilizando heapify
// retorna 1 em caso de sucesso, 0 se vazio e -1 se os parametros forem invalidos
int removeMinHeap(HeapMinimo *heap, int *vencimento){
    if (heap == NULL || heap->tam < 0 || heap->tam > CAP || vencimento == NULL) // verifica se heap e vencimento são inválidos
        return -1;
    if (heap->tam == 0) // verifica se o heap está vazio
        return 0;
     
    *vencimento = heap->dados[0]; // consulta o menor vencimento (raiz do heap)

    heap->tam--; // decrementa o tamanho do heap

    if(heap->tam > 0){
        heap->dados[0] = heap->dados[heap->tam]; // move o último elemento para a raiz do heap
        heapify(heap->dados, heap->tam, 0); // reorganiza o heap para manter a propriedade de heap mínimo
    }

    return 1; // sucesso
}

// retorna a quantidade de elementos armazenados no heap ou -1 se invalido
int tamanhoHeap(const HeapMinimo *heap){
    if (heap == NULL || heap->tam < 0 || heap->tam > CAP) // verifica se o heap é inválido
        return -1;
    
    return heap->tam;
}
