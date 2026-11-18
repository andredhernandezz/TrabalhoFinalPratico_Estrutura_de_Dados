#include <stdio.h>
#include "estoque.h"

static int falhas = 0;

static void confere(const char *desc, int obtido, int esperado)
{
    if (obtido == esperado)
        printf("[ok]    %s\n", desc);
    else
    {
        printf("[FALHA] %s (obtido %d, esperado %d)\n", desc, obtido, esperado);
        falhas++;
    }
}

static void secao(const char *titulo)
{
    printf("\n== %s ==\n", titulo);
}

int main(void)
{
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

    // verifica se o vencimento e calculado corretamente
    secao("preparaIngrediente: calculo do vencimento (preparo no turno 10)");

    confere("prepara carne", preparaIngrediente(&e, CARNE, 10), 1);
    consultaMinHeap(&e.carne, &v);
    confere("vencimento da carne", v, 10 + VENC_CARNE);

    confere("prepara ovo", preparaIngrediente(&e, OVO, 10), 1);
    consultaMinHeap(&e.ovo, &v);
    confere("vencimento do ovo", v, 10 + VENC_OVO);

    confere("prepara bacon", preparaIngrediente(&e, BACON, 10), 1);
    consultaMinHeap(&e.bacon, &v);
    confere("vencimento do bacon", v, 10 + VENC_BACON);

    // testa a consulta dos turnos restantes de validade
    secao("consultaValidade");

    // reinicia o estoque para comecar os testes sem ingredientes
    inicializaEstoque(&e);

    // verifica a consulta com os heaps vazios
    confere("carne vazia retorna -1", consultaValidade(&e, CARNE, 1), -1);
    confere("ovo vazio retorna -1", consultaValidade(&e, OVO, 1), -1);
    confere("bacon vazio retorna -1", consultaValidade(&e, BACON, 1), -1);

    // prepara uma unidade de cada ingrediente perecivel no turno 10
    preparaIngrediente(&e, CARNE, 10); // vence no turno 14
    preparaIngrediente(&e, OVO, 10);   // vence no turno 14
    preparaIngrediente(&e, BACON, 10); // vence no turno 14

    // verifica a validade logo apos o preparo
    confere("carne tem 4 turnos", consultaValidade(&e, CARNE, 10), 4);
    confere("ovo tem 4 turnos", consultaValidade(&e, OVO, 10), 4);
    confere("bacon tem 4 turnos", consultaValidade(&e, BACON, 10), 4);

    // verifica se a validade diminui conforme o turno avanca
    confere("carne no turno 11 tem 3 turnos", consultaValidade(&e, CARNE, 11), 3);
    confere("carne no turno 12 tem 2 turnos", consultaValidade(&e, CARNE, 12), 2);
    confere("carne no turno 13 tem 1 turno", consultaValidade(&e, CARNE, 13), 1);

    // verifica se a consulta nao remove ingredientes do estoque
    confere("carne continua no estoque", consultaEstoque(&e, CARNE), 1);
    confere("ovo continua no estoque", consultaEstoque(&e, OVO), 1);
    confere("bacon continua no estoque", consultaEstoque(&e, BACON), 1);

    // prepara mais carnes com vencimentos diferentes
    preparaIngrediente(&e, CARNE, 15); // vence no turno 19
    preparaIngrediente(&e, CARNE, 12); // vence no turno 16

    // o heap deve consultar a carne que vence primeiro
    confere("carne com vencimento mais proximo", consultaValidade(&e, CARNE, 13), 1);
    confere("tres carnes continuam no estoque", consultaEstoque(&e, CARNE), 3);

    // descarta a carne que vence no turno 14
    verificaVencimentos(&e, 14);

    confere("carne mais antiga foi descartada", consultaEstoque(&e, CARNE), 2);
    confere("proxima carne vence em 2 turnos", consultaValidade(&e, CARNE, 14), 2);

    // verifica parametros invalidos
    confere("estoque NULL retorna -2", consultaValidade(NULL, CARNE, 1), -2);
    confere("ingrediente invalido retorna -2",
            consultaValidade(&e, (IngredienteEstoque)99, 1), -2);
    confere("ingrediente nao perecivel retorna -2",
            consultaValidade(&e, CEBOLA, 1), -2);
    confere("turno negativo retorna -2", consultaValidade(&e, CARNE, -1), -2);

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
    preparaIngrediente(&e, CARNE, 5); // vence 9
    preparaIngrediente(&e, CARNE, 1); // vence 5
    preparaIngrediente(&e, CARNE, 3); // vence 7
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
    preparaIngrediente(&e, CARNE, 1); // vence 5
    preparaIngrediente(&e, CARNE, 2); // vence 6
    preparaIngrediente(&e, CARNE, 3); // vence 7
    confere("turno 4: nada descartado", verificaVencimentos(&e, 4), 0);
    confere("turno 4: carne = 3", consultaEstoque(&e, CARNE), 3);
    confere("turno 5: descarta 1 (vencimento == turno)", verificaVencimentos(&e, 5), 1);
    confere("turno 5: carne = 2", consultaEstoque(&e, CARNE), 2);
    confere("turno 5 de novo: nada a descartar", verificaVencimentos(&e, 5), 0);
    confere("turno 7: descarta 2 de uma vez", verificaVencimentos(&e, 7), 2);
    confere("turno 7: carne = 0", consultaEstoque(&e, CARNE), 0);
    confere("heap vazio: nada a descartar", verificaVencimentos(&e, 99), 0);

    // verifica se o heap descarta primeiro os ingredientes mais antigos
    // independentemente da ordem em que foram preparados
    secao("verificaVencimentos: heap fora de ordem de insercao");

    inicializaEstoque(&e);

    preparaIngrediente(&e, OVO, 9);  // vence no turno 13
    preparaIngrediente(&e, OVO, 1);  // vence no turno 5
    preparaIngrediente(&e, OVO, 2);  // vence no turno 6
    preparaIngrediente(&e, OVO, 20); // vence no turno 24

    // no turno 6, os ovos com vencimentos 5 e 6 devem ser descartados
    confere("turno 6: descarta 2 ovos", verificaVencimentos(&e, 6), 2);
    confere("ovo = 2", consultaEstoque(&e, OVO), 2);

    // o proximo vencimento deve ser o do ovo preparado no turno 9
    consultaMinHeap(&e.ovo, &v);
    confere("menor restante e 13", v, 13);

    // descarta o ovo que vence no turno 13
    confere("turno 13: descarta 1", verificaVencimentos(&e, 13), 1);

    // o ultimo ovo deve vencer no turno 24
    consultaMinHeap(&e.ovo, &v);
    confere("menor restante e 24", v, 24);

    // verifica o vencimento simultaneo dos tres ingredientes pereciveis
    secao("verificaVencimentos: os tres pereciveis");

    inicializaEstoque(&e);

    // prepara todos os ingredientes no turno 10
    preparaIngrediente(&e, CARNE, 10);
    preparaIngrediente(&e, OVO, 10);
    preparaIngrediente(&e, BACON, 10);
    preparaIngrediente(&e, CEBOLA, 10);

    // nenhum ingrediente deve vencer antes do turno 14
    confere("turno 12: nada vence", verificaVencimentos(&e, 12), 0);
    confere("turno 13: nada vence", verificaVencimentos(&e, 13), 0);

    // os tres pereciveis vencem simultaneamente no turno 14
    confere("turno 14: descarta os tres", verificaVencimentos(&e, 14), 3);

    // verifica se todos foram removidos
    confere("carne = 0", consultaEstoque(&e, CARNE), 0);
    confere("ovo = 0", consultaEstoque(&e, OVO), 0);
    confere("bacon = 0", consultaEstoque(&e, BACON), 0);

    // ingredientes nao pereciveis nao devem ser descartados
    confere("cebola nao e afetada (perene)", consultaEstoque(&e, CEBOLA), 1);

    secao("sequencia: preparar, vencer, consumir");
    inicializaEstoque(&e);
    // prepara dois bacons em turnos diferentes
    preparaIngrediente(&e, BACON, 1); // vence no turno 5
    preparaIngrediente(&e, BACON, 4); // vence no turno 8

    // descarta apenas o primeiro bacon
    verificaVencimentos(&e, 6);
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