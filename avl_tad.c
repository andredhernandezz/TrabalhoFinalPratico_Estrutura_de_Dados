#include <stdlib.h>
#include "avl_tad.h"

static Contador contador; // armazena as metricas de desempenho da AVL

static int erroInsercao = 0; // registra se a ultima insercao falhou por falta de memoria

// zera os contadores de rotacoes, nodos inseridos e comparacoes
void avlZerarMetricas(void){
    contador.rotacoes = 0;
    contador.nosInserido = 0;
    contador.comparacoes = 0;
}

// retorna os valores atuais dos contadores de desempenho da AVL
Contador avlObterMetricas(void){
    return contador;
}

// retorna a altura do nodo, considerando 0 para arvore vazia e 1 para folha
int avlAltura(const NodoAvl *no) {
    if (no == NULL) return 0;
    if ((no->esq == NULL) && (no->dir == NULL)){
        return 1;
    }
    return no->altura;
}

/* FB = h(esq) - h(dir). Positivo = pesado à esq; negativo = pesado à dir. */
int avlFb(const NodoAvl *no) {
    if (no == NULL) return 0;
    return avlAltura(no->esq) - avlAltura(no->dir);
}

/* Recalcula e armazena a altura de 'no' com base nas alturas dos filhos.
   Chamado sempre que um filho muda (inserção, remoção ou rotação). */
void avlAtualizarAltura(NodoAvl *no) {
    int ae = avlAltura(no->esq);
    int ad = avlAltura(no->dir);
    if (ae > ad)
        no->altura = 1 + ae;
    else
        no->altura = 1 + ad;
}

/* Rotação simples à DIREITA (caso LL).
   x sobe; B (filho dir de x) passa para filho esq de y.
   Atualiza y primeiro (ficou abaixo), depois x. */
NodoAvl* avlRotDir(NodoAvl *y) {
    NodoAvl *x = y->esq;
    NodoAvl *B = x->dir;
    x->dir = y;
    y->esq = B;
    avlAtualizarAltura(y);
    avlAtualizarAltura(x);
    contador.rotacoes++;
    return x;
}

/* Rotação simples à ESQUERDA (caso RR). Simétrica à anterior.
   y sobe; B (filho esq de y) passa para filho dir de x. */
NodoAvl* avlRotEsq(NodoAvl *x) {
    NodoAvl *y = x->dir;
    NodoAvl *B = y->esq;
    y->esq = x;
    x->dir = B;
    avlAtualizarAltura(x);
    avlAtualizarAltura(y);
    contador.rotacoes++;
    return y;
}

/* Verifica o FB e aplica a rotação correta.
   Usada tanto na inserção quanto na remoção. */
NodoAvl* avlReequilibrar(NodoAvl *raiz) {
    avlAtualizarAltura(raiz);
    int fb = avlFb(raiz);

    /* LL: pesado à esq, filho esq também pesado à esq ou equilibrado */
    if (fb == +2 && avlFb(raiz->esq) >= 0)
        return avlRotDir(raiz);

    /* RR: pesado à dir, filho dir também pesado à dir ou equilibrado */
    if (fb == -2 && avlFb(raiz->dir) <= 0)
        return avlRotEsq(raiz);

    /* LR: pesado à esq, filho esq pesado à dir → transforma LR em LL */
    if (fb == +2 && avlFb(raiz->esq) < 0) {
        raiz->esq = avlRotEsq(raiz->esq);
        return avlRotDir(raiz);
    }

    /* RL: pesado à dir, filho dir pesado à esq → transforma RL em RR */
    if (fb == -2 && avlFb(raiz->dir) > 0) {
        raiz->dir = avlRotDir(raiz->dir);
        return avlRotEsq(raiz);
    }

    return raiz;   /* sem desequilíbrio */
}

/* Busca um pedido pelo codigo, contabilizando os nodos visitados.
   Assim como na ABP, percorre apenas um caminho da arvore.
   O balanceamento da AVL garante o custo O(log n). */
NodoAvl* avlBuscar(NodoAvl *raiz, int cod) {
    if (raiz == NULL)
        return NULL;
    if (cod == raiz->dado.cod){
        contador.comparacoes++;
        return raiz;
    }
    if (cod < raiz->dado.cod){
        contador.comparacoes++;
        return avlBuscar(raiz->esq, cod);
    }
    contador.comparacoes++;
    return avlBuscar(raiz->dir, cod);
}

/* Insere um pedido na AVL e informa se houve falha de alocacao.
   Em caso de erro, preserva os ponteiros da arvore existente. */
static NodoAvl* avlInserirAux(NodoAvl *raiz, Pedido valor, int *erro) {
    if (raiz == NULL) {
        NodoAvl *novo = (NodoAvl*) malloc(sizeof(NodoAvl));

        // se nao houver memoria, informa o erro e nao cria o nodo
        if (novo == NULL) {
            *erro = 1;
            return NULL;
        }

        contador.nosInserido++;

        novo->dado = valor;
        novo->esq = NULL;
        novo->dir = NULL;
        novo->altura = 1;

        return novo;
    }

    // procura a posicao correta para inserir o pedido
    if (valor.cod < raiz->dado.cod) {
        NodoAvl *filho = avlInserirAux(raiz->esq, valor, erro);

        // se a alocacao falhou, mantem o filho original
        if (*erro)
            return raiz;

        raiz->esq = filho;
    }
    else if (valor.cod > raiz->dado.cod) {
        NodoAvl *filho = avlInserirAux(raiz->dir, valor, erro);

        // se a alocacao falhou, mantem o filho original
        if (*erro)
            return raiz;

        raiz->dir = filho;
    }
    else {
        // pedidos com codigo duplicado nao sao inseridos
        return raiz;
    }

    // atualiza as alturas e corrige possiveis desequilibrios
    return avlReequilibrar(raiz);
}

// Inicia a insercao de um pedido e registra possiveis falhas de alocacao. 
NodoAvl* avlInserir(NodoAvl *raiz, Pedido valor) {
    erroInsercao = 0;

    return avlInserirAux(raiz, valor, &erroInsercao);
}

/* Informa se a ultima insercao falhou por falta de memoria. */
int avlErroInsercao(void) {
    return erroInsercao;
}

/* Destroi toda a árvore em pós-ordem.
   Uso: raiz = avlDestruir(raiz); */
NodoAvl* avlDestruir(NodoAvl *raiz) {
    if (raiz == NULL) return NULL;
    raiz->esq = avlDestruir(raiz->esq);
    raiz->dir = avlDestruir(raiz->dir);
    free(raiz);
    return NULL;
}