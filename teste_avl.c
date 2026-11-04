#include <stdio.h>
#include "avl_tad.h"

/* >>> ADAPTE estas duas linhas ao seu contador de rotacoes <<<
   ex. com global:  #define ZERAR_ROTACOES() (rotacoes = 0)
                    #define LER_ROTACOES()   (rotacoes)
   ex. com struct:  use o campo da sua struct de metricas       */
#define ZERAR_ROTACOES()  /* ... */
#define LER_ROTACOES()    (-1)

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

int main(void) {
    int falhas = 0;

    falhas += !testaCaso("LL (3,2,1)",   3, 2, 1, 1, 2);
    falhas += !testaCaso("RR (1,2,3)",   1, 2, 3, 1, 2);
    falhas += !testaCaso("LR (3,1,2)",   3, 1, 2, 2, 2);
    falhas += !testaCaso("RL (1,3,2)",   1, 3, 2, 2, 2);
    falhas += !testaCaso("Apendice B",   1001, 1042, 2017, 1, 2);

    printf("\n%s\n", falhas == 0 ? "Todos os testes passaram."
                                 : "Ha testes falhando.");
    return falhas != 0;
}