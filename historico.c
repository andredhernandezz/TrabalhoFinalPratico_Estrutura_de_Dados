#include <stdio.h>
#include <stdlib.h>
#include <timer.h>




ListaContig copia;                       // uma variável, não um ponteiro
copiaListaContig(&historico, &copia);    // cópia fora do intervalo cronometrado

timer_iniciar(&t);
quicksortPontuacao(&copia);              // só o algoritmo é medido
double s = timer_parar(&t);

destroiListaContig(&copia);