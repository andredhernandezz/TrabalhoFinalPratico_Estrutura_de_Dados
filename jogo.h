#ifndef JOGO_H
#define JOGO_H

#include "estoque.h"
#include "pilha_tad.h"
#include "fila_tad.h"
#include "lista_tad.h"
#include "hash_tad.h"
#include "avl_tad.h"
#include "metricas.h"

#define MAX_CLIENTES 10
#define MAX_PACIENCIA 15

typedef enum{
    JOGO_ANDAMENTO, // partida em andamento
    JOGO_VITORIA, // partida encerrada com sucesso (todos os clientes atendidos)
    JOGO_DERROTA, // partida encerrada com derrota (cliente desistiu)
    JOGO_ABANDONO // partida encerrada por abandono do jogador
} StatusJogo;

// resultados possíveis das ações realizadas pelo jogador
typedef enum{
    ACAO_SUCESSO, // ação realizada com sucesso
    ACAO_SIMBOLO_INVALIDO, // símbolo digitado é inválido
    ACAO_SEM_ESTOQUE, // não há ingredientes suficientes em estoque
    ACAO_ESTOQUE_CHEIO, // limite de armazenamento do ingrediente atingido
    ACAO_PRATO_FECHADO, // o prato já foi fechado e não pode ser modificado
    ACAO_PRATO_VAZIO, // não há ingredientes para remover
    ACAO_JOGO_ENCERRADO, // a partida já foi encerrada e não pode ser continuada
    ACAO_ERRO_MEMORIA, // erro de alocação de memória durante a execução da ação
    ACAO_ERRO_INTERNO // ocorreu uma situação inesperada na lógica do jogo
} ResultadoAcao;

typedef struct{
    Estoque estoque; // ingredientes disponiveis
    PilhaEnc prato; // ingredientes do hamburguer em montagem
    FilaEnc clientes; // fila de clientes esperando atendimento
    ListaEnc receita; // receita esperada para o cliente atual
    TabelaHash cache; // cache de pedidos consultados durante a busca
    MetricasBusca buscas; // métricas das buscas na AVL

    NodoAvl *raizPedidos; // arvore AVL com os pedidos
    
    int turno; // turno atual da partida
    int paciencia; // paciencia restante do cliente atual
    int pacienciaMax; // paciencia maxima de cada cliente
    int atendidos; // quantidade de clientes atendidos
    int totalClientes; // quantidade total de clientes da partida
    int pontuacao; // pontuacao acumulada durante a partida

    StatusJogo status; // estado atual da partida

} Jogo;

// inicializa uma partida, criando os clientes e preparando as estruturas.
// retorna 1 se inicializou corretamente e 0 em caso de erro.
int iniciaJogo(Jogo *j, NodoAvl *raiz, int numClientes, int pacienciaMax);

// prepara um ingrediente identificado pelo seu símbolo.
ResultadoAcao jogoPrepara(Jogo *j, char simbolo);

// adiciona um ingrediente ao hambúrguer.
// verifica estoque, validade e regras de montagem.
ResultadoAcao jogoAdiciona(Jogo *j, char simbolo);

// remove o ingrediente que está no topo do hambúrguer.
ResultadoAcao jogoRemove(Jogo *j);

// encerra a partida por decisão do jogador.
ResultadoAcao jogoAbandona(Jogo *j);

// libera a memória utilizada pelas estruturas da partida.
void destroiJogo(Jogo *j);

#endif