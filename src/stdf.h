#ifndef SCREEN_FUNC_H
#define SCREEN_FUNC_H

#include "splash_screen.h"

// Riga e colonna dove verra' scritto il prossimo carattere. Sono
// variabili statiche a livello di file: solo le funzioni dentro
// questo file possono leggerle o modificarle, tramite stampa_carattere.
static int riga_corrente = 0;
static int colonna_corrente = 0;

/*clear the screen in vga mode with char*/
void clear_screen(char character_to_use_to_fill_the_screen);
void print(char carattere);
void print_at_color(char *str, int row, int col, char color);

void stampa_stringa(char *stringa);
void present();
void n_to_hex_str(unsigned long long valore, char *buffer_destinazione);
void n_to_str(unsigned long long valore, char *buffer_destinazione);
void stampa_regioni_utilizzabili(void);

#endif