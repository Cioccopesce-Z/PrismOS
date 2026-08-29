#include "kernel.h"
#include "idt.h"

// Riga e colonna dove verra' scritto il prossimo carattere. Sono
// variabili statiche a livello di file: solo le funzioni dentro
// questo file possono leggerle o modificarle, tramite stampa_carattere.
static int riga_corrente = 0;
static int colonna_corrente = 0;

void clear_screen(){
    char *video_memory = (char*) VIDEO_MEMORY_ADDRESS;
    int numero_totale_di_caselle = SCREEN_WIDTH_IN_CHARACTERS * SCREEN_HEIGHT_IN_CHARACTERS;

    for(int indice_casella = 0; indice_casella < numero_totale_di_caselle; indice_casella++){
        video_memory[indice_casella * 2] = ' ';
        video_memory[indice_casella * 2 + 1] = DEFAULT_COLOR_ATTRIBUTE;
    }

    riga_corrente = 0;
    colonna_corrente = 0;
}

// Scrive un carattere alla posizione attuale del cursore e sposta il
// cursore in avanti. Versione volutamente semplice: quando lo schermo
// si riempie, ricomincia dall'alto invece di far scorrere il contenuto
// verso l'alto (scorrimento vero, da implementare in seguito quando
// affronteremo la stampa di stringhe in modo completo).
void stampa_carattere(char carattere){
    char *video_memory = (char*) VIDEO_MEMORY_ADDRESS;

    if(carattere == '\n'){
        riga_corrente = riga_corrente + 1;
        colonna_corrente = 0;
    } else {
        int indice_casella = riga_corrente * SCREEN_WIDTH_IN_CHARACTERS + colonna_corrente;
        video_memory[indice_casella * 2] = carattere;
        video_memory[indice_casella * 2 + 1] = DEFAULT_COLOR_ATTRIBUTE;

        colonna_corrente = colonna_corrente + 1;
        if(colonna_corrente >= SCREEN_WIDTH_IN_CHARACTERS){
            colonna_corrente = 0;
            riga_corrente = riga_corrente + 1;
        }
    }

    if(riga_corrente >= SCREEN_HEIGHT_IN_CHARACTERS){
        riga_corrente = 0;
    }
}

void print(char *str) {
    char *video_memory = (char*) VIDEO_MEMORY_ADDRESS;

    int j = 0;

    while (str[j] != '\0') {
        video_memory[j * 2] = str[j];
        video_memory[j * 2 + 1] = DEFAULT_COLOR_ATTRIBUTE;
        j++;
    }
}


void print_at_color(char *str, int row, int col, char color) {
    char *video_memory = (char*) VIDEO_MEMORY_ADDRESS;

    int j = 0;

    while (str[j] != '\0') {
        int pos = (row * SCREEN_WIDTH + col + j) * 2;

        video_memory[pos] = str[j];
        video_memory[pos + 1] = color;

        j++;
    }
}

void prism_screen(){

    print_at_color(
        "+------------------------------------------------------------------------------+",
        1, 0, VGA_LIGHT_CYAN
    );

    print_at_color(
        "|                                                                              |",
        2, 0, VGA_LIGHT_BLUE
    );

    print_at_color(
        "|                            P R I S M   O S                                   |",
        3, 0, VGA_LIGHT_MAGENTA
    );

    print_at_color(
        "|                                                                              |",
        4, 0, VGA_LIGHT_BLUE
    );

    print_at_color(
        "|                        +------------------------+                            |",
        5, 0, VGA_LIGHT_CYAN
    );

    print_at_color(
        "|                        |                        |                            |",
        6, 0, VGA_LIGHT_CYAN
    );

    print_at_color(
        "|                        |      P R I S M         |                            |",
        7, 0, VGA_LIGHT_MAGENTA
    );

    print_at_color(
        "|                        |         O S            |                            |",
        8, 0, VGA_LIGHT_RED
    );

    print_at_color(
        "|                        |                        |                            |",
        9, 0, VGA_LIGHT_CYAN
    );

    print_at_color(
        "|                        +------------------------+                            |",
        10, 0, VGA_LIGHT_CYAN
    );

    print_at_color(
        "|                                                                              |",
        11, 0, VGA_LIGHT_BLUE
    );

    print_at_color(
        "|                                                                              |",
        12, 0, VGA_LIGHT_BLUE
    );

    print_at_color(
        "|                       SYSTEM INITIALIZED                                     |",
        13, 0, VGA_LIGHT_GREEN
    );

    print_at_color(
        "|                                                                              |",
        14, 0, VGA_LIGHT_BLUE
    );

    print_at_color(
        "|                         [ KERNEL OK ]                                        |",
        15, 0, VGA_LIGHT_GREEN
    );

    print_at_color(
        "|                         [ IDT OK ]                                           |",
        16, 0, VGA_LIGHT_GREEN
    );

    print_at_color(
        "|                         [ VGA OK ]                                           |",
        17, 0, VGA_LIGHT_GREEN
    );

    print_at_color(
        "|                                                                              |",
        18, 0, VGA_LIGHT_BLUE
    );

    print_at_color(
        "|                                                                              |",
        19, 0, VGA_RED
    );

    print_at_color(
        "|                                                                              |",
        20, 0, VGA_YELLOW
    );

    print_at_color(
        "|                                                                              |",
        21, 0, VGA_LIGHT_BLUE
    );

    print_at_color(
        "+------------------------------------------------------------------------------+",
        22, 0, VGA_LIGHT_MAGENTA
    );

    print_at_color(
        " ",
        23, 0, VGA_RED
    );

    print_at_color(
        "                         PRISM OS v0.1",
        24, 0, VGA_LIGHT_GRAY
    );
}



void kernel_main() {

    clear_screen();

    idt_inizializza();

    prism_screen();

    while(1) {
        __asm__ volatile ("hlt");
    }
}
