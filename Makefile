# ============================================================================
# MAKEFILE - DADO BURGER
# Trabalho Final de Estruturas de Dados - UFRGS - 2026/2
# ============================================================================
#
# OBJETIVO
#   Automatizar a compilacao do jogo e dos programas de teste, evitando que
#   seja necessario digitar manualmente todos os arquivos .c no GCC.
#
# PRE-REQUISITOS
#   - GNU Make;
#   - GCC (ou compilador compativel com as opcoes utilizadas);
#   - Terminal aberto na pasta que contem este Makefile.
#
# COMANDOS PRINCIPAIS
#   make              Compila o executavel principal.
#   make help         Mostra um resumo dos comandos disponiveis.
#   make run          Compila e executa o jogo com os arquivos padrao.
#   make run PEDIDOS=outro_pedidos.txt HISTORICO=outro_historico.txt
#                     Executa o jogo com arquivos informados pelo usuario.
#   make test         Compila e executa todos os testes automatizados.
#   make test-avl     Executa somente os testes da AVL.
#   make test-estoque Executa somente os testes do estoque.
#   make test-heap    Executa somente os testes do heap.
#   make test-hash    Executa somente os testes da tabela hash.
#   make debug        Recompila o jogo com simbolos de depuracao.
#   make release      Recompila o jogo com otimizacao.
#   make clean        Remove executaveis, objetos e dependencias gerados.
#   make rebuild      Limpa e compila tudo novamente.
#
# TESTE DA HASH
#   O teste da hash utiliza os arquivos de pedidos e historico. Por padrao,
#   utiliza pedidos.txt e historico.txt, mas e possivel informar outros nomes:
#
#   make test-hash PEDIDOS=pedidos_teste.txt HISTORICO=historico_teste.txt
#
#   Tambem e possivel ajustar a quantidade de buscas e de codigos distintos:
#
#   make test-hash BUSCAS=5000 DISTINTOS=500
#
# OBSERVACOES
#   - O Make recompila apenas os arquivos-fonte que foram alterados e os
#     modulos que dependem deles. Os arquivos .d registram essas dependencias.
#   - Os arquivos .o e .d sao intermediarios gerados automaticamente.
#   - Os programas de teste sao independentes do executavel principal.
#   - O alvo 'test' executa os testes existentes no projeto; ele nao substitui
#     testes manuais de integracao do jogo.
#   - Para usar outro compilador, execute, por exemplo: make CC=clang
#   - Para alterar opcoes, execute, por exemplo: make CFLAGS="-std=c99 -Wall"
#
# IMPORTANTE SOBRE O MAKEFILE
#   As linhas de comandos abaixo das regras precisam comecar com TAB, nao
#   espacos. Isso e uma exigencia da sintaxe tradicional do GNU Make.
#
# ============================================================================

# Compilador C. Pode ser substituido na chamada: make CC=clang
CC = gcc

# Opcoes padrao: C99, avisos importantes e otimizacao moderada.
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -O2

# Bibliotecas adicionais necessarias na etapa de ligacao.
LDLIBS = -lm

# Extensao do executavel conforme o sistema operacional.
ifeq ($(OS),Windows_NT)
    EXE_EXT = .exe
else
    EXE_EXT =
endif

# Define o comando de remoção conforme o sistema operacional.
ifeq ($(OS),Windows_NT)
    RM = del /F /Q
else
    RM = rm -f
endif

# Nome do executavel principal.
TARGET = dado_burger$(EXE_EXT)

# Arquivos utilizados pelo programa principal.
MAIN_SOURCE = main.c
CORE_SOURCES = avl_tad.c \
               busca.c \
               carregamento.c \
               estoque.c \
               fila_tad.c \
               hash_tad.c \
               heap_tad.c \
               historico.c \
               jogo.c \
               lista_contig_tad.c \
               lista_tad.c \
               metricas.c \
               ordenacao.c \
               pilha_tad.c \
               timer.c

# Converte os nomes dos fontes em nomes de arquivos-objeto.
CORE_OBJECTS = $(CORE_SOURCES:.c=.o)
MAIN_OBJECT = $(MAIN_SOURCE:.c=.o)

# Fontes e objetos dos programas de teste.
TEST_AVL_OBJECT = teste_avl.o
TEST_ESTOQUE_OBJECT = teste_estoque.o
TEST_HEAP_OBJECT = teste_heap.o
TEST_HASH_OBJECT = teste_hash.o

# Arquivos de entrada padrao. Podem ser sobrescritos na linha de comando.
PEDIDOS ?= pedidos.txt
HISTORICO ?= historico.txt
BUSCAS ?= 1000
DISTINTOS ?= 300

# Arquivos de dependencias automaticas gerados pelo compilador.
DEPFILES = $(CORE_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d) \
           $(TEST_AVL_OBJECT:.o=.d) $(TEST_ESTOQUE_OBJECT:.o=.d) \
           $(TEST_HEAP_OBJECT:.o=.d) $(TEST_HASH_OBJECT:.o=.d)

# O primeiro alvo e o padrao: executar apenas 'make' compila o jogo.
.DEFAULT_GOAL := all

.PHONY: all help run test test-avl test-estoque test-heap test-hash \
        debug release clean rebuild

# Compilacao principal.
all: $(TARGET)

$(TARGET): $(MAIN_OBJECT) $(CORE_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

# Regra generica para transformar qualquer arquivo .c em .o.
# -MMD e -MP geram dependencias dos headers automaticamente.
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# ---------------------------------------------------------------------------
# PROGRAMAS DE TESTE
# ---------------------------------------------------------------------------

# AVL: utiliza somente a implementacao da arvore.
teste_avl$(EXE_EXT): $(TEST_AVL_OBJECT) avl_tad.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

test-avl: teste_avl$(EXE_EXT)
	./teste_avl$(EXE_EXT)

# Estoque: depende do TAD de estoque e do heap utilizado pelos pereciveis.
teste_estoque$(EXE_EXT): $(TEST_ESTOQUE_OBJECT) estoque.o heap_tad.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

test-estoque: teste_estoque$(EXE_EXT)
	./teste_estoque$(EXE_EXT)

# Heap: teste isolado da estrutura de heap.
teste_heap$(EXE_EXT): $(TEST_HEAP_OBJECT) heap_tad.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

test-heap: teste_heap$(EXE_EXT)
	./teste_heap$(EXE_EXT)

# Hash: utiliza a AVL, o carregamento de pedidos, o cache e as metricas.
teste_hash$(EXE_EXT): $(TEST_HASH_OBJECT) $(filter-out jogo.o estoque.o fila_tad.o lista_tad.o pilha_tad.o heap_tad.o,$(CORE_OBJECTS))
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

test-hash: teste_hash$(EXE_EXT)
	./teste_hash$(EXE_EXT) "$(PEDIDOS)" "$(HISTORICO)" $(BUSCAS) $(DISTINTOS)

# Compila e executa todos os testes disponiveis.
test: test-avl test-estoque test-heap test-hash

# ---------------------------------------------------------------------------
# EXECUCAO E CONFIGURACOES DE COMPILACAO
# ---------------------------------------------------------------------------

# Executa o programa principal com os arquivos de entrada informados.
run: $(TARGET)
	./$(TARGET) "$(PEDIDOS)" "$(HISTORICO)"

# Recompila com informacoes uteis para depuracao (sem otimizacao).
debug:
	$(MAKE) clean
	$(MAKE) CFLAGS="-std=c99 -Wall -Wextra -Wpedantic -O0 -g3" all

# Recompila com otimizacao para uma versao de entrega.
release:
	$(MAKE) clean
	$(MAKE) CFLAGS="-std=c99 -Wall -Wextra -Wpedantic -O2 -DNDEBUG" all

# Remove arquivos gerados, preservando fontes, headers e arquivos de entrada.
clean:
	$(RM) $(TARGET) teste_avl$(EXE_EXT) teste_estoque$(EXE_EXT) \
	       teste_heap$(EXE_EXT) teste_hash$(EXE_EXT) \
	       $(MAIN_OBJECT) $(CORE_OBJECTS) $(TEST_AVL_OBJECT) \
	       $(TEST_ESTOQUE_OBJECT) $(TEST_HEAP_OBJECT) $(TEST_HASH_OBJECT) \
	       $(DEPFILES) medicoes.log medicoes_teste_hash.log

# Forca uma compilacao completa.
rebuild: clean all

# Ajuda resumida exibida por 'make help'.
help:
	@echo "Dado Burger - comandos disponiveis:"
	@echo "  make              Compila o jogo"
	@echo "  make run          Compila e executa o jogo"
	@echo "  make test         Executa todos os testes"
	@echo "  make test-avl     Testa a AVL"
	@echo "  make test-estoque Testa o estoque"
	@echo "  make test-heap    Testa o heap"
	@echo "  make test-hash    Testa a tabela hash"
	@echo "  make debug        Compila para depuracao"
	@echo "  make release      Compila com otimizacao"
	@echo "  make clean        Remove arquivos gerados"
	@echo "  make rebuild      Limpa e compila novamente"
	@echo ""
	@echo "Personalize os arquivos com: make run PEDIDOS=arquivo.txt HISTORICO=historico.txt"

# Inclui dependencias de headers geradas durante a compilacao.
# O '-' evita erro na primeira compilacao, quando os .d ainda nao existem.
-include $(DEPFILES)
