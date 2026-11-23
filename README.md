# Dado Burger

Trabalho Prático Final — INF01203 Estruturas de Dados (UFRGS, 2026/2, Turma C, prof. Dennis Giovani Balreira)

Grupo 6: André Delgado Hernandez (615633) e Germano Garlet de Bortoli (613108)

Jogo por turnos em C (C99) em que o jogador atende uma fila de clientes montando hambúrgueres. Estruturas usadas: AVL (pedidos), lista contígua (histórico), tabela hash (cache de pedidos), lista encadeada (receita), pilha (montagem), fila (clientes) e heap mínimo (perecíveis).

## Compilação

Com Make:

    make

Ou direto com o gcc:

    gcc -std=c99 -Wall -o dado_burger main.c avl_tad.c busca.c carregamento.c estoque.c fila_tad.c hash_tad.c heap_tad.c historico.c jogo.c lista_contig_tad.c lista_tad.c metricas.c ordenacao.c pilha_tad.c timer.c -lm

Observação: os módulos têm o sufixo `_tad` (por exemplo `avl_tad.c` em vez de `avl.c`) e há módulos extras (`busca.c`, `carregamento.c`, `estoque.c`, `historico.c`, `jogo.c`, `metricas.c`, `ordenacao.c`), por isso a lista de arquivos difere da do Apêndice C do enunciado.

## Execução

O programa recebe os arquivos pela linha de comando (não há nomes fixos):

    ./dado_burger <arquivo_de_pedidos> <arquivo_de_historico>

Exemplos:

    ./dado_burger pedidos.txt historico.txt                  # desenvolvimento
    ./dado_burger pedidos_final.txt historico_final.txt      # apresentação

Os arquivos devem estar no diretório de onde o programa é executado. Se o arquivo de histórico não existir, o programa começa com histórico vazio e o cria ao fim da primeira partida. Com Make: `make run PEDIDOS=pedidos.txt HISTORICO=historico.txt`.

## Como jogar

Menu: 1 = Novo jogo, 2 = Como jogar, 3 = Histórico, 0 = Sair. Dentro da partida: 1 = preparar ingrediente, 2 = adicionar ao hambúrguer, 3 = remover o topo, 4 = consultar pedido (não consome turno), 0 = abandonar. Ingredientes são informados pelo símbolo (k, m, p, a, s, q, c, t, n, o, b, z). A tela "Como jogar" traz a tabela completa.

Parâmetros: 10 clientes por partida, paciência de 15 turnos, validade de 4 turnos para carne, ovo frito e bacon, hash com m = 101 e h(c) = c mod m, ordenação do histórico por quicksort (com mergesort cronometrado para comparação).

## Arquivos gerados

- `medicoes.log`: criado/sobrescrito a cada execução. Contém as métricas de carregamento (AVL e histórico), os tempos de ordenação a cada acesso ao Histórico e o Relatório de Desempenho ao fim de cada partida.
- `historico.txt` (ou o arquivo passado em `argv[2]`): recebe uma nova entrada (nome e pontuação) ao fim de cada partida. A ordenação nunca altera o arquivo.

## Testes

    make test        # AVL, estoque, heap e hash
    make clean       # remove binários, objetos e medicoes.log

## Extra: interface gráfica (Raylib)

Opcional, não faz parte da compilação padrão. Requer a Raylib; instruções em `LEIA-ME_GUI.md` (`build_gui.bat` no Windows).

## Declaração de uso de IA

Consta no relatório em PDF.
