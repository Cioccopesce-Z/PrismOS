// file: src/io.c
#include "io.h"

// "outb" e "inb" sono istruzioni del processore x86 che non esistono
// in C puro: per questo vanno scritte in assembly incorporato (inline
// assembly), direttamente dentro una funzione C.
void scrivi_byte_su_porta(unsigned short numero_porta, unsigned char valore){
    __asm__ volatile ("outb %0, %1" : : "a"(valore), "Nd"(numero_porta));
}

unsigned char leggi_byte_da_porta(unsigned short numero_porta){
    unsigned char valore;
    __asm__ volatile ("inb %1, %0" : "=a"(valore) : "Nd"(numero_porta));
    return valore;
}