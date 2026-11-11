#include <stdio.h>
#include "estoque.h"

static int falhas = 0;

static void confere(const char *desc, int obtido, int esperado){
    if (obtido == esperado)
        printf("[ok]    %s\n", desc);
    else {
        printf("[FALHA] %s (obtido %d, esperado %d)\n", desc, obtido, esperado);
        falhas++;
    }
}

static void secao(const char *titulo){
    printf("\n== %s ==\n", titulo);
}

int main(void){
    Estoque e;
    int v;

    secao("inicializaEstoque");
    inicializaEstoque(&e);
    confere("carne comeca com 0", consultaEstoque(&e, CARNE), 0);
    confere("ovo comeca com 0", consultaEstoque(&e, OVO), 0);
    confere("bacon comeca com 0", consultaEstoque(&e, BACON), 0);
    confere("queijo suico comeca com 0", consultaEstoque(&e, QUEIJO_SUICO), 0);
    confere("queijo cheddar comeca com 0", consultaEstoque(&e, QUEIJO_CHEDDAR), 0);
    confere("cebola comeca com 0", consultaEstoque(&e, CEBOLA), 0);
    confere("tomate comeca com 0", consultaEstoque(&e, TOMATE), 0);

    secao("preparaIngrediente: calculo do vencimento (preparo no turno 10)");
    confere("prepara carne", preparaIngrediente(&e, CARNE, 10), 1);
    consultaMinHeap(&e.carne, &v);
    confere("carne vence no turno 14", v, 14);
    confere("prepara ovo", preparaIngrediente(&e, OVO, 10), 1);
    consultaMinHeap(&e.ovo, &v);
    confere("ovo vence no turno 13", v, 13);
    confere("prepara bacon", preparaIngrediente(&e, BACON, 10), 1);
    consultaMinHeap(&e.bacon, &v);
    confere("bacon vence no turno 15", v, 15);

    secao("preparaIngrediente: perenes");
    preparaIngrediente(&e, QUEIJO_SUICO, 1);
    preparaIngrediente(&e, QUEIJO_SUICO, 2);
    preparaIngrediente(&e, QUEIJO_CHEDDAR, 3);
    preparaIngrediente(&e, CEBOLA, 4);
    preparaIngrediente(&e, TOMATE, 5);
    preparaIngrediente(&e, TOMATE, 6);
    preparaIngrediente(&e, TOMATE, 7);
    confere("queijo suico = 2", consultaEstoque(&e, QUEIJO_SUICO), 2);
    confere("queijo cheddar = 1", consultaEstoque(&e, QUEIJO_CHEDDAR), 1);
    confere("cebola = 1", consultaEstoque(&e, CEBOLA), 1);
    confere("tomate = 3", consultaEstoque(&e, TOMATE), 3);

    secao("consomeIngrediente: perecivel consome o que vence primeiro");
    inicializaEstoque(&e);
    preparaIngrediente(&e, CARNE, 5);   // vence 9
    preparaIngrediente(&e, CARNE, 1);   // vence 5
    preparaIngrediente(&e, CARNE, 3);   // vence 7
    confere("carne = 3", consultaEstoque(&e, CARNE), 3);
    confere("consome 1 carne", consomeIngrediente(&e, CARNE), 1);
    confere("carne = 2", consultaEstoque(&e, CARNE), 2);
    consultaMinHeap(&e.carne, &v);
    confere("a de vencimento 5 saiu; menor agora e 7", v, 7);
    confere("consome mais 1", consomeIngrediente(&e, CARNE), 1);
    consultaMinHeap(&e.carne, &v);
    confere("menor agora e 9", v, 9);
    confere("consome a ultima", consomeIngrediente(&e, CARNE), 1);
    confere("carne = 0", consultaEstoque(&e, CARNE), 0);
    confere("consumir com heap vazio retorna 0", consomeIngrediente(&e, CARNE), 0);
    confere("carne continua 0 (sem negativo)", consultaEstoque(&e, CARNE), 0);
    confere("consumir ovo vazio retorna 0", consomeIngrediente(&e, OVO), 0);
    confere("consumir bacon vazio retorna 0", consomeIngrediente(&e, BACON), 0);

    secao("consomeIngrediente: perenes");
    confere("cebola vazia retorna 0", consomeIngrediente(&e, CEBOLA), 0);
    confere("cebola continua 0", consultaEstoque(&e, CEBOLA), 0);
    preparaIngrediente(&e, CEBOLA, 1);
    preparaIngrediente(&e, CEBOLA, 2);
    confere("cebola = 2", consultaEstoque(&e, CEBOLA), 2);
    confere("consome cebola", consomeIngrediente(&e, CEBOLA), 1);
    confere("cebola = 1", consultaEstoque(&e, CEBOLA), 1);
    confere("consome cebola", consomeIngrediente(&e, CEBOLA), 1);
    confere("cebola = 0", consultaEstoque(&e, CEBOLA), 0);
    confere("queijo suico vazio retorna 0", consomeIngrediente(&e, QUEIJO_SUICO), 0);
    confere("queijo cheddar vazio retorna 0", consomeIngrediente(&e, QUEIJO_CHEDDAR), 0);
    confere("tomate vazio retorna 0", consomeIngrediente(&e, TOMATE), 0);
    preparaIngrediente(&e, TOMATE, 1);
    confere("consome tomate", consomeIngrediente(&e, TOMATE), 1);
    confere("tomate = 0", consultaEstoque(&e, TOMATE), 0);

    secao("verificaVencimentos: exemplo do enunciado (carne preparada nos turnos 1, 2 e 3)");
    inicializaEstoque(&e);
    preparaIngrediente(&e, CARNE, 1);   // vence 5
    preparaIngrediente(&e, CARNE, 2);   // vence 6
    preparaIngrediente(&e, CARNE, 3);   // vence 7
    confere("turno 4: nada descartado", verificaVencimentos(&e, 4), 0);
    confere("turno 4: carne = 3", consultaEstoque(&e, CARNE), 3);
    confere("turno 5: descarta 1 (vencimento == turno)", verificaVencimentos(&e, 5), 1);
    confere("turno 5: carne = 2", consultaEstoque(&e, CARNE), 2);
    confere("turno 5 de novo: nada a descartar", verificaVencimentos(&e, 5), 0);
    confere("turno 7: descarta 2 de uma vez", verificaVencimentos(&e, 7), 2);
    confere("turno 7: carne = 0", consultaEstoque(&e, CARNE), 0);
    confere("heap vazio: nada a descartar", verificaVencimentos(&e, 99), 0);

    secao("verificaVencimentos: heap fora de ordem de insercao");
    inicializaEstoque(&e);
    preparaIngrediente(&e, OVO, 9);     // vence 12
    preparaIngrediente(&e, OVO, 1);     // vence 4
    preparaIngrediente(&e, OVO, 2);     // vence 5
    preparaIngrediente(&e, OVO, 20);    // vence 23
    confere("turno 6: descarta 2 (vencimentos 4 e 5)", verificaVencimentos(&e, 6), 2);
    confere("ovo = 2", consultaEstoque(&e, OVO), 2);
    consultaMinHeap(&e.ovo, &v);
    confere("menor restante e 12", v, 12);
    confere("turno 12: descarta 1", verificaVencimentos(&e, 12), 1);
    consultaMinHeap(&e.ovo, &v);
    confere("menor restante e 23", v, 23);

    secao("verificaVencimentos: os tres pereciveis (preparo no turno 10: 14, 13, 15)");
    inicializaEstoque(&e);
    preparaIngrediente(&e, CARNE, 10);
    preparaIngrediente(&e, OVO, 10);
    preparaIngrediente(&e, BACON, 10);
    preparaIngrediente(&e, CEBOLA, 10);
    confere("turno 12: nada vence", verificaVencimentos(&e, 12), 0);
    confere("turno 13: so o ovo", verificaVencimentos(&e, 13), 1);
    confere("ovo = 0", consultaEstoque(&e, OVO), 0);
    confere("carne ainda = 1", consultaEstoque(&e, CARNE), 1);
    confere("bacon ainda = 1", consultaEstoque(&e, BACON), 1);
    confere("turno 15: carne e bacon de uma vez", verificaVencimentos(&e, 15), 2);
    confere("carne = 0", consultaEstoque(&e, CARNE), 0);
    confere("bacon = 0", consultaEstoque(&e, BACON), 0);
    confere("cebola nao e afetada (perene)", consultaEstoque(&e, CEBOLA), 1);

    secao("sequencia: preparar, vencer, consumir");
    inicializaEstoque(&e);
    preparaIngrediente(&e, BACON, 1);   // vence 6
    preparaIngrediente(&e, BACON, 4);   // vence 9
    verificaVencimentos(&e, 6);             // a primeira vence
    confere("sobrou 1 bacon", consultaEstoque(&e, BACON), 1);
    confere("consome o que sobrou", consomeIngrediente(&e, BACON), 1);
    confere("heap vazio apos consumir (consulta retorna 0)", consultaMinHeap(&e.bacon, &v), 0);

    secao("erros e valores invalidos");
    confere("prepara com estoque NULL", preparaIngrediente(NULL, CARNE, 1), -1);
    confere("consome com estoque NULL", consomeIngrediente(NULL, CARNE), -1);
    confere("verifica com estoque NULL", verificaVencimentos(NULL, 1), -1);
    confere("consulta com estoque NULL", consultaEstoque(NULL, CARNE), -1);
    confere("prepara ingrediente invalido", preparaIngrediente(&e, (IngredienteEstoque)99, 1), -1);
    confere("consome ingrediente invalido", consomeIngrediente(&e, (IngredienteEstoque)99), -1);
    confere("consulta ingrediente invalido", consultaEstoque(&e, (IngredienteEstoque)99), -1);
    inicializaEstoque(NULL);
    confere("inicializaEstoque(NULL) nao quebra", 1, 1);

    secao("capacidade maxima");
    inicializaEstoque(&e);
    int ok = 1;
    for (int i = 0; i < CAP; i++)
        if (preparaIngrediente(&e, BACON, i) != 1)
            ok = 0;
    confere("1024 bacons preparados com sucesso", ok, 1);
    confere("bacon 1025 com heap cheio retorna 0", preparaIngrediente(&e, BACON, 0), 0);
    confere("bacon continua com 1024", consultaEstoque(&e, BACON), CAP);

    secao("reinicializar");
    inicializaEstoque(&e);
    confere("bacon volta a 0", consultaEstoque(&e, BACON), 0);

    if (falhas == 0)
        printf("\nTodos os testes passaram.\n");
    else
        printf("\n%d falha(s).\n", falhas);
    return falhas != 0;
}