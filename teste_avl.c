#include <stdio.h>
#include "avl_tad.h"
#define ZERAR_ROTACOES()  avlZerarMetricas()
#define LER_ROTACOES()    (avlObterMetricas().rotacoes)

/* Insere 3 codigos numa arvore nova e confere rotacoes e altura. */
static int testaCaso(const char *nome, int a, int b, int c,
                     int rotEsperadas, int altEsperada) {
    NodoAvl *raiz = NULL;
    Pedido p;
    int cods[3] = { a, b, c };

    p.ingredientes[0] = '\0';
    ZERAR_ROTACOES();

    for (int i = 0; i < 3; i++) {
        p.cod = cods[i];
        raiz = avlInserir(raiz, p);
    }

    int rot = LER_ROTACOES();
    int alt = avlAltura(raiz);
    int ok = (rot == rotEsperadas && alt == altEsperada);

    printf("%-14s rotacoes=%d (esp %d)  altura=%d (esp %d)  %s\n",
           nome, rot, rotEsperadas, alt, altEsperada, ok ? "OK" : "FALHOU");

    avlDestruir(raiz);
    return ok;
}

/* Insere os codigos dados (pode haver repetidos) e confere nosInserido. */
static int testaNosInseridos(const char *nome, const int *cods, int n,
                             int esperado) {
    NodoAvl *raiz = NULL;
    Pedido p;

    p.ingredientes[0] = '\0';
    avlZerarMetricas();

    for (int i = 0; i < n; i++) {
        p.cod = cods[i];
        raiz = avlInserir(raiz, p);
    }

    int obtido = avlObterMetricas().nosInserido;
    int ok = (obtido == esperado);

    printf("%-14s nos=%d (esp %d)  %s\n",
           nome, obtido, esperado, ok ? "OK" : "FALHOU");

    avlDestruir(raiz);
    return ok;
}

/* Busca 'cod' na arvore dada e confere o numero de comparacoes. */
static int testaBusca(const char *nome, NodoAvl *raiz, int cod,
                      int esperado, int deveAchar) {
    avlZerarMetricas();
    NodoAvl *r = avlBuscar(raiz, cod);

    int obtido = avlObterMetricas().comparacoes;
    int achou = (r != NULL);
    int ok = (obtido == esperado && achou == deveAchar);

    printf("%-16s comp=%d (esp %d)  achou=%d (esp %d)  %s\n",
           nome, obtido, esperado, achou, deveAchar, ok ? "OK" : "FALHOU");
    return ok;
}


int main(void) {
    int falhas = 0;

    falhas += !testaCaso("LL (3,2,1)",   3, 2, 1, 1, 2);
    falhas += !testaCaso("RR (1,2,3)",   1, 2, 3, 1, 2);
    falhas += !testaCaso("LR (3,1,2)",   3, 1, 2, 2, 2);
    falhas += !testaCaso("RL (1,3,2)",   1, 3, 2, 2, 2);
    falhas += !testaCaso("Apendice B",   1001, 1042, 2017, 1, 2);


        int semRepetidos[] = { 3, 2, 1 };
    int comRepetido[]  = { 3, 2, 1, 2 };

    falhas += !testaNosInseridos("3 distintos", semRepetidos, 3, 3);
    falhas += !testaNosInseridos("com duplicado", comRepetido, 4, 3);

        NodoAvl *arv = NULL;
    Pedido q;
    q.ingredientes[0] = '\0';
    int cods[] = { 1001, 1042, 2017 };
    for (int i = 0; i < 3; i++) {
        q.cod = cods[i];
        arv = avlInserir(arv, q);
    }

    falhas += !testaBusca("busca 1042 (raiz)", arv, 1042, 1, 1);
    falhas += !testaBusca("busca 1001",        arv, 1001, 2, 1);
    falhas += !testaBusca("busca 2017",        arv, 2017, 2, 1);
    falhas += !testaBusca("busca 9999",        arv, 9999, 2, 0);

    avlDestruir(arv);

    

    printf("\n%s\n", falhas == 0 ? "Todos os testes passaram."
                                 : "Ha testes falhando.");
    return falhas != 0;
}