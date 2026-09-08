#ifndef LOC_H
#define LOC_H

#define ALLINEAMENTO_ALLOCAZIONE 4ULL
#define NUMERO_MASSIMO_BLOCCHI_ALLOCATI 128

void *alloc(unsigned long long byte);
void libera(void *puntatore);

#endif