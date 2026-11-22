@echo off
REM Requer w64devkit/MinGW com Raylib instalada (ver LEIA-ME_GUI.md)
gcc -std=gnu99 -O2 main_gui.c avl_tad.c busca.c carregamento.c estoque.c fila_tad.c hash_tad.c heap_tad.c historico.c jogo.c lista_contig_tad.c lista_tad.c metricas.c ordenacao.c pilha_tad.c timer.c -o dado_burger_gui.exe -lraylib -lopengl32 -lgdi32 -lwinmm -lm
if errorlevel 1 (echo FALHOU) else (echo OK: dado_burger_gui.exe pedidos.txt historico.txt)
