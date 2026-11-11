
#include <stdio.h>
#include "heap_tad.h"

int main(void){
    HeapMinimo heap;
    int vencimento;

    // 1. Inicializacao
    inicializaHeap(&heap);

    printf("Heap vazio? %d\n", heapVazio(&heap));

    // 2. Insercoes
    insereHeap(&heap, 8);
    insereHeap(&heap, 5);
    insereHeap(&heap, 12);
    insereHeap(&heap, 3);
    insereHeap(&heap, 6);

    printf("Tamanho: %d\n", tamanhoHeap(&heap));

    // 3. Consulta do minimo
    if (consultaMinHeap(&heap, &vencimento) == 1)
        printf("Menor vencimento: %d\n", vencimento);

    // 4. Remocoes sucessivas
    printf("\nRemovendo elementos:\n");

    while (!heapVazio(&heap)){
        removeMinHeap(&heap, &vencimento);
        printf("Removido: %d\n", vencimento);
    }

    // 5. Tentativa de remocao com heap vazio
    printf("\nTentando remover de heap vazio:\n");

    int resultado = removeMinHeap(&heap, &vencimento);

    printf("Resultado: %d\n", resultado);

    
    // 6. Teste com vencimentos iguais
    printf("\nTeste com vencimentos iguais:\n");

    inicializaHeap(&heap);

    insereHeap(&heap, 5);
    insereHeap(&heap, 5);
    insereHeap(&heap, 3);
    insereHeap(&heap, 5);
    insereHeap(&heap, 3);

    while (!heapVazio(&heap)){
        removeMinHeap(&heap, &vencimento);
        printf("Removido: %d\n", vencimento);
    }

    
    // 7. Teste de capacidade maxima
    printf("\nTeste de capacidade maxima:\n");

    inicializaHeap(&heap);

    int inseridos = 0;

    for (int i = 0; i < CAP; i++){
        if (insereHeap(&heap, i) == 1)
            inseridos++;
    }

    printf("Elementos inseridos: %d\n", inseridos);
    printf("Tamanho do heap: %d\n", tamanhoHeap(&heap));

    resultado = insereHeap(&heap, 9999);

    printf("Tentativa de inserir com heap cheio: %d\n", resultado);


    return 0;
}


