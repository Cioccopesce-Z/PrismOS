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
float itof(int value);

int ctoi(char c);
float ctof(char c);

int ftoi(float fvalue);
char ftoc(float fvalue);

// ---------------------------------------------------------------------
// Conversioni da puntatore: qui non si converte un valore, si
// reinterpreta l'INDIRIZZO stesso come se fosse un numero qualsiasi.
// Non leggono la memoria a cui il puntatore punta: per quello serve
// la dereferenziazione (l'operatore *), un'operazione diversa. Vedi i
// commenti in type_conv.c per i limiti di ciascuna, in particolare
// per indirizzi superiori ai 2 GB.
// ---------------------------------------------------------------------

int ptoi(void *pt);
char ptoc(void *pt);
float ptof(void *pt);

#endif