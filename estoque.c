#include <stddef.h>
#include "estoque.h"

void inicializaEstoque(Estoque *estoque){
    if (estoque == NULL) // verifica se estoque é inválido
        return;

    inicializaHeap(&estoque->carne);
    inicializaHeap(&estoque->ovo);
    inicializaHeap(&estoque->bacon);
    estoque->queijoSuico = 0;
    estoque->queijoCheddar = 0;
    estoque->cebola = 0;
    estoque->tomate = 0;
}

int preparaIngrediente(Estoque *estoque, IngredienteEstoque ing, int turnoAtual){
    if (estoque == NULL || turnoAtual < 0) // verifica se estoque ou turnoAtual são invalidos
        return -1;
    
    switch(ing){

        // pereciveis: guarda o turno de vencimento no heap do ingrediente
        case CARNE:
            return insereHeap(&estoque->carne, turnoAtual + VENC_CARNE);
        case OVO:
            return insereHeap(&estoque->ovo, turnoAtual + VENC_OVO);
        case BACON:
            return insereHeap(&estoque->bacon, turnoAtual + VENC_BACON);

        // nao pereciveis: incrementa o contador do ingrediente
        case QUEIJO_SUICO:
            estoque->queijoSuico++;
            return 1;
        case QUEIJO_CHEDDAR:
            estoque->queijoCheddar++;   
            return 1;
        case CEBOLA:
            estoque->cebola++;
            return 1;
        case TOMATE:
            estoque->tomate++;
            return 1;

        default:
            return -1; // ingrediente fora do enum
    }
}

int consomeIngrediente(Estoque *estoque, IngredienteEstoque ing){
    if (estoque == NULL) // verfica se estoque é inválido
        return -1;
    
    int vencimento; // para armazenar o turno de vencimento removido do heap    

    switch(ing){
        // pereciveis: remove a raiz do heap (menor turno de vencimento)
        case CARNE:
            return removeMinHeap(&estoque->carne, &vencimento);
        case OVO:
            return removeMinHeap(&estoque->ovo, &vencimento);
        case BACON:
            return removeMinHeap(&estoque->bacon, &vencimento);
        // nao pereciveis: decrementa o contador do ingrediente
        case QUEIJO_SUICO:
            if (estoque->queijoSuico > 0){
                estoque->queijoSuico--;
                return 1;
            }
            return 0;
        case QUEIJO_CHEDDAR:
            if (estoque->queijoCheddar > 0){
                estoque->queijoCheddar--;
                return 1;
            }
            return 0;
        case CEBOLA:
            if (estoque->cebola > 0){
                estoque->cebola--;
                return 1;
            }
            return 0;
        case TOMATE:
            if (estoque->tomate > 0){
                estoque->tomate--;
                return 1;
            }
            return 0;
        
        default:
            return -1; // ingrediente fora do enum
    }
}

static int descartaVencidos(HeapMinimo *heap, int turnoAtual){
    if (heap == NULL || turnoAtual < 0) // verifica se heap ou turnoAtual são invalidos
        return -1;
    
    int descartados = 0;
    int vencimento;

    // enquanto houver elementos no heap e o menor vencimento for menor ou igual ao turnoAtual
    while (consultaMinHeap(heap, &vencimento) == 1 && vencimento <= turnoAtual){
        removeMinHeap(heap, &vencimento);
        descartados++;
    }

    return descartados;
}

int verificaVencimentos(Estoque *estoque, int turnoAtual){
    if (estoque == NULL || turnoAtual < 0) // verifica se estoque ou turnoAtual são invalidos
        return -1;

    return descartaVencidos(&estoque->carne, turnoAtual) +
           descartaVencidos(&estoque->ovo, turnoAtual) +
           descartaVencidos(&estoque->bacon, turnoAtual);
}

int consultaEstoque(const Estoque *estoque, IngredienteEstoque ing){
    if (estoque == NULL) // verifica se estoque é invalido
        return -1;
    
    switch(ing){
        // pereciveis: retorna o tamanho do heap do ingrediente
        case CARNE:
            return tamanhoHeap(&estoque->carne);
        case OVO:
            return tamanhoHeap(&estoque->ovo);
        case BACON:
            return tamanhoHeap(&estoque->bacon);
        // nao pereciveis: retorna o contador do ingrediente
        case QUEIJO_SUICO:
            return estoque->queijoSuico;
        case QUEIJO_CHEDDAR:
            return estoque->queijoCheddar;
        case CEBOLA:
            return estoque->cebola;
        case TOMATE:
            return estoque->tomate;
        
        default: 
            return -1; // ingrediente fora do enum
    }
}

// consulta a validade restante do ingrediente perecivel
int consultaValidade(const Estoque *estoque, IngredienteEstoque ing, int turnoAtual) {
    int vencimento;
    int resultado;

    // verifica se os parametros recebidos sao validos
    if (estoque == NULL || turnoAtual < 0)
        return -2;

    // consulta o heap correspondente ao ingrediente
    switch (ing) {
        case CARNE:
            resultado = consultaMinHeap(&estoque->carne, &vencimento);
            break;

        case OVO:
            resultado = consultaMinHeap(&estoque->ovo, &vencimento);
            break;

        case BACON:
            resultado = consultaMinHeap(&estoque->bacon, &vencimento);
            break;

        default:
            // a funcao deve ser utilizada apenas com ingredientes pereciveis
            return -2;
    }

    // verifica se ocorreu um erro na consulta
    if (resultado == -1)
        return -2;

    // verifica se nao existe nenhuma unidade preparada
    if (resultado == 0)
        return -1;

    // calcula quantos turnos faltam para o vencimento
    return vencimento - turnoAtual;
}
