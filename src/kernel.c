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

void kernel_main(){
    clear_screen();
    idt_inizializza();
    print("kill yourself");

    // Da qui il flusso principale non fa piu' nulla attivamente: resta
    // fermo. "hlt" mette in pausa il processore fino alla prossima
    // interruzione, invece di consumare energia in un ciclo vuoto come
    // faceva "jmp $" in kernel.asm. Le interruzioni della tastiera,
    // pero', continueranno ad arrivare e a essere gestite in
    // sottofondo da gestore_interruzione_tastiera.
    while(1){
        __asm__ volatile ("hlt");
    }
}




