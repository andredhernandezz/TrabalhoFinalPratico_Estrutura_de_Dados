#ifndef LISTA_CONTIG_TAD_H
#define LISTA_CONTIG_TAD_H

typedef struct{
    char nome[50];
    int pontuacao;
} NovoHistorico; // Uma partida registrada


typedef struct{
    NovoHistorico *dados;
    int tamanho;
    int capacidade;
} ListaContig;


int inicializaListaContig(ListaContig *lista);
int insereFimContig(ListaContig *lista, NovoHistorico input);
int obtemContig(const ListaContig *lista, int indice, NovoHistorico *novo);
int tamanhoListaContig(const ListaContig *lista);
int copiaListaContig(const ListaContig *origem, ListaContig *destino); // para ordenacao 
void destroiListaContig(ListaContig *lista);

#endif