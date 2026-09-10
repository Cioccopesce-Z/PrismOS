// file: src/type_conv.h
#ifndef TYPE_CONV_H
#define TYPE_CONV_H

// ---------------------------------------------------------------------
// Conversioni dirette: trasformano un VALORE in un altro tipo,
// mantenendone (dove possibile) il significato numerico. Sono
// l'equivalente dei normali cast del C, incapsulati in una funzione
// con un nome esplicito, cosi' da poterle usare anche come argomento
// di altre funzioni.
// ---------------------------------------------------------------------

char itoc(int value);

int ctoi(char c);

int ptoi(void *pt);
char ptoc(void *pt);

void *itop(int value);
void *ctop(char c);

#endif