// file: src/io.h
#ifndef IO_H
#define IO_H

// Scrive un singolo byte su una porta di input/output del processore.
// Le porte di I/O sono un canale di comunicazione separato dalla
// memoria RAM, usato dai componenti hardware come il controller delle
// interruzioni e la tastiera: non ci si scrive con un normale
// puntatore, serve un'istruzione apposita del processore.
void scrivi_byte_su_porta(unsigned short numero_porta, unsigned char valore);

// Legge un singolo byte da una porta di input/output del processore.
unsigned char leggi_byte_da_porta(unsigned short numero_porta);

#endif