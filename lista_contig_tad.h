#ifndef LISTA_CONTIG_TAD_H
#define LISTA_CONTIG_TAD_H

// estrutura que representa uma partida registrada no historico
typedef struct{
    char nome[50]; // nome do jogador
    int pontuacao; // numero de clientes atendidos
} NovoHistorico;

// lista contigua utilizada para armazenar o historico de partidas
typedef struct{
    NovoHistorico *dados; // vetor de registros
    int tamanho; // quantidade de registros armazenados
    int capacidade; // quantidade de posicoes alocadas
} ListaContig;


// inicializa uma lista contigua vazia
int inicializaListaContig(ListaContig *lista);

// insere um registro no final da lista
int insereFimContig(ListaContig *lista, NovoHistorico input);

// consulta um registro pelo indice
int obtemContig(const ListaContig *lista, int indice, NovoHistorico *novo);

// retorna a quantidade de registros armazenados
int tamanhoListaContig(const ListaContig *lista);

// cria uma copia independente da lista para ordenacao
int copiaListaContig(const ListaContig *origem, ListaContig *destino);

// libera a memoria utilizada pela lista
void destroiListaContig(ListaContig *lista);

#endif