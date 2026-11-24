#ifndef TIMER_H
#define TIMER_H

#include <time.h>

// estrutura utilizada para medir o tempo de CPU
typedef struct {
    clock_t inicio; // instante inicial da medicao
} Timer;

// inicia a medicao
void timer_iniciar(Timer *t);

// retorna o tempo de CPU decorrido em segundos
double timer_parar(Timer *t);

#endif