# Dado Burger - interface grafica (Raylib)

**Por que Raylib:** C puro (sem C++), API pequena, um unico arquivo de interface, binario unico no Windows. SDL2 exigiria mais codigo para desenhar formas/texto.

## Instalar (Windows)
1. Baixe o **raylib installer** (github.com/raysan5/raylib/releases, "raylib-x.y_win64_mingw-w64") e instale (ja traz o compilador w64devkit).
2. Abra o terminal "w64devkit" / o atalho do raylib na pasta do projeto.
3. `build_gui.bat`  e depois  `dado_burger_gui.exe pedidos.txt historico.txt`

## Controles
Letra = adicionar | Shift+letra ou botao COZINHAR = preparar | Backspace = remover topo | Esc = abandonar | mouse tambem funciona.

## O que mudou no codigo original
Nada. Todos os .c/.h existentes ficaram intactos; foram adicionados apenas `main_gui.c`, `build_gui.bat`, `Makefile.gui`. O `main.c` (terminal) continua compilando com o `Makefile` normal.

## Icones (opcional)
Coloque PNGs em `assets/ico_<simbolo>.png` (ex.: `ico_k.png` ketchup, `ico_n.png` carne). Sem eles, a interface usa circulos coloridos.

## Aviso importante (regra da disciplina)
O enunciado exige declarar no relatorio todo uso de IA. Declare: interface grafica (main_gui.c) gerada com IA; TADs e metricas nao foram alterados.
