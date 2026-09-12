#ifndef SCREEN_FUNC_H
#define SCREEN_FUNC_H

#include "splash_screen.h"
#include "kernel.h"

// Riga e colonna dove verra' scritto il prossimo carattere. Sono
// variabili statiche a livello di file: solo le funzioni dentro
// questo file possono leggerle o modificarle, tramite stampa_carattere.
void sposta_cursore_indietro(void);

/*clear the screen in vga mode with char*/
void clear_screen(char character_to_use_to_fill_the_screen);
void fprint(const char *formato, ...);
void print(char carattere);
void print_at_color(char *str, int row, int col, char color);

void stampa_stringa(char *stringa);
void present();



//debug effettive
void stampa_dump_memoria(unsigned char *indirizzo_di_partenza, unsigned int numero_di_byte);
void stampa_regioni_utilizzabili(void);

extern size_t exit_code;

#endif