#include "timer.h"

// registra o instante inicial da medicao
void timer_iniciar(Timer *t) {
    t->inicio = clock();
}

// retorna o tempo de CPU decorrido desde o inicio da medicao, em segundos
double timer_parar(Timer *t) {
    return (double)(clock() - t->inicio) / CLOCKS_PER_SEC;
}