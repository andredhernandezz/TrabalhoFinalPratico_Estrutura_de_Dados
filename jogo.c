#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jogo.h"
#include "timer.h"

// conta quantos pedidos tem na AVL
static int contaPedidos(NodoAvl *raiz){
    if(raiz == NULL) 
        return 0;
    
    return 1 + contaPedidos(raiz->esq) + contaPedidos(raiz->dir);
}

// preenche um vetor com os códigos em ordem crescente
static void preencheCodigos(NodoAvl *raiz, int *vetor, int *posicao){
    if(raiz == NULL) 
        return;
    
    preencheCodigos(raiz->esq, vetor, posicao);

    vetor[*posicao] = raiz->dado.cod;
    (*posicao)++;
    preencheCodigos(raiz->dir, vetor, posicao);
}

// sorteia um índice do vetor de códigos entre 0 e tamanho-1
static int sorteiaIndice(int tamanho) {
    return rand() % tamanho;
}

// consulta primeiro hash e, em caso de falta, busca na AVL
// retorna 1 se encontrou o pedido, 0 se não encontrou, e -1 em caso de erro
static int consultaPedido(Jogo *j, int cod, Pedido *pedido){
    if (hashBuscar(&j->cache, cod, pedido)) {
        return 1; // encontrado no hash  
    }
    
    Contador antes = avlObterMetricas();

    Timer t;
    timer_iniciar(&t);

    NodoAvl *nodo = avlBuscar(j->raizPedidos, cod);

    double tempo = timer_parar(&t);

    Contador depois = avlObterMetricas();

    j->buscas.buscas++;
    j->buscas.comparacoes += depois.comparacoes - antes.comparacoes;
    j->buscas.tempo += tempo;

    if (nodo == NULL)
        return 0; // não encontrado na AVL
    
    *pedido = nodo->dado; // copia o pedido encontrado

    if (!hashInserir(&j->cache, *pedido)) {
        return -1; // erro de memória ao inserir no hash
    }

    return 1; // encontrado na AVL e inserido no hash
}

void destroiJogo(Jogo *j) {
    if (j == NULL) 
        return;

    esvaziaFila(&j->clientes);
    esvaziaPilha(&j->prato);
    destroiLista(&j->receita);
    hashDestruir(&j->cache);

    memset(j, 0, sizeof(Jogo)); // limpa a estrutura do jogo
}

int iniciaJogo(Jogo *j, NodoAvl *raiz, int numClientes, int pacienciaMax) {
    if (j == NULL || raiz == NULL || numClientes <= 0 || numClientes > MAX_CLIENTES || pacienciaMax <= 0 || pacienciaMax > MAX_PACIENCIA)
        return 0; // parâmetros inválidos
    
    memset(j, 0, sizeof(Jogo)); // inicializa a estrutura do jogo

    j->raizPedidos = raiz;
    j->turno = 1;
    j->paciencia = pacienciaMax;
    j->pacienciaMax = pacienciaMax;
    j->atendidos = 0;
    j->totalClientes = numClientes;
    j->status = JOGO_ANDAMENTO;

    inicializaEstoque(&j->estoque);
    inicializaPilha(&j->prato);
    inicializaFila(&j->clientes);
    inicializaLista(&j->receita);

    if (!hashInicializar(&j->cache, HASH_M_PADRAO)) {
        destroiJogo(j);
        return 0; // falha ao inicializar o hash 
    }

    // descobre quantos pedidos tem na AVL
    int totalPedidos = contaPedidos(raiz);

    if (totalPedidos == 0) {
        destroiJogo(j);
        return 0; // não há pedidos
    }

    // cria um vetor temporario com os codigos existentes
    int *codigos = (int *)malloc(totalPedidos * sizeof(int));

    if (codigos == NULL) {
        destroiJogo(j);
        return 0; // falha de memória
    }

    int posicao = 0;
    preencheCodigos(raiz, codigos, &posicao);

    // gera os clientes e seus respectivos pedidos
    for (int i = 0; i < numClientes; i++) {
        Cliente cliente;

        snprintf(cliente.nome, sizeof(cliente.nome), "Cliente %d", i + 1);

        int indice = sorteiaIndice(totalPedidos);
        int cod = codigos[indice];

        int resultado = consultaPedido(j, cod, &cliente.pedido);

        if (resultado != 1){
            free(codigos);
            destroiJogo(j);
            return 0; // falha ao consultar o pedido
        }
         
        if (enqueue(&j->clientes, cliente) != 1) {
            free(codigos);
            destroiJogo(j);
            return 0; // falha ao enfileirar o cliente
        }
    }
    free(codigos);

    // prepara a receita do primeiro cliente
    Cliente clienteAtual;

    if(frenteFila(&j->clientes, &clienteAtual) != 1 || !montaReceita(&j->receita, &clienteAtual.pedido.ingredientes)) {
        destroiJogo(j);
        return 0; // falha ao obter o cliente atual
    }

    // verifica vencimentos no inicio do primeiro turno
    verificaVencimentos(&j->estoque, j->turno);

    return 1; // sucesso ao inicializar o jogo
}