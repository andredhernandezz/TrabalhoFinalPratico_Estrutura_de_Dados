/* Teste da hash de cache com a AVL real.
   Uso: ./teste_hash pedidos.txt historico.txt [buscas] [codigos_distintos]
   Sorteia codigos (com repeticao), busca via hash->AVL e imprime o relatorio. */
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "avl_tad.h"
#include "carregamento.h"
#include "historico.h"
#include "hash_tad.h"
#include "busca.h"
#include "metricas.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <pedidos> <historico> [buscas] [codigos_distintos]\n", argv[0]);
        return 1;
    }
    int nBuscas   = (argc > 3) ? atoi(argv[3]) : 1000;
    int distintos = (argc > 4) ? atoi(argv[4]) : 300;

    NodoAvl *raiz = NULL;
    avlZerarMetricas();
    Timer t;
    timer_iniciar(&t);
    int lidos = carregarPedidos(argv[1], &raiz);
    double s = timer_parar(&t);
    if (lidos < 0) {
        printf("Erro ao abrir %s\n", argv[1]);
        return 1;
    }
    Contador mAvl = avlObterMetricas();

    ListaContig hist;
    inicializaListaContig(&hist);
    int nHist = carregarHistorico(argv[2], &hist);

    MetricasCarga carga = { mAvl.nosInserido, avlAltura(raiz), mAvl.rotacoes, s, nHist };

    TabelaHash cache;
    hashInicializar(&cache, HASH_M_PADRAO);
    MetricasBusca mb = { 0, 0, 0.0 };

    srand(12345);
    int achou = 0;
    for (int i = 0; i < nBuscas; i++) {
        Pedido p;
        int cod = 1 + rand() % distintos;
        achou += buscarPedido(raiz, &cache, cod, &p, &mb);
    }
    printf("buscas feitas: %d | encontradas: %d\n", nBuscas, achou);

    MetricasHash mh = { cache.m, cache.n, cache.hits, cache.misses, cache.colisoes };
    metricasRelatorioPartida("medicoes_teste_hash.log", &carga, &mb, &mh);

    hashDestruir(&cache);
    destroiListaContig(&hist);
    avlDestruir(raiz);
    return 0;
}