#ifndef LOC_H
#define LOC_H

#include "kernel.h"

#define ALLINEAMENTO_ALLOCAZIONE 4ULL
#define NUMERO_MASSIMO_BLOCCHI_ALLOCATI 128

void *alloc(unsigned long long byte);
void *ralloc(void* pt, size_t dimension);
void libera(void *puntatore);


#endif