// file: src/idt.h
#ifndef IDT_H
#define IDT_H

// Una singola voce della IDT. Il formato di questa struttura non e'
// una scelta del programmatore: e' imposto byte per byte dal
// processore x86 in modalita' protetta a 32 bit. Per questo la
// struttura e' marcata "packed": dice al compilatore di non inserire
// byte di riempimento tra i campi per allinearli in memoria, come
// farebbe normalmente, perche' il processore si aspetta questi byte
// esattamente uno dopo l'altro, senza spazi vuoti.
typedef struct {
    unsigned short indirizzo_gestore_parte_bassa; // bit 0-15 dell'indirizzo della funzione da eseguire
    unsigned short selettore_segmento_codice;      // quale segmento di codice usare
    unsigned char  riservato;                      // byte non utilizzato, deve restare a zero
    unsigned char  attributi_e_tipo;               // tipo di interruzione e livello di privilegio richiesto
    unsigned short indirizzo_gestore_parte_alta;   // bit 16-31 dell'indirizzo della funzione da eseguire
} __attribute__((packed)) voce_della_idt;

// Struttura passata all'istruzione LIDT per dire al processore dove si
// trova la tabella e quanto e' grande. Stesso concetto della struttura
// "gdt_descriptor" che hai gia' in boot.asm per la GDT.
typedef struct {
    unsigned short dimensione_tabella_meno_uno;
    unsigned int   indirizzo_tabella;
} __attribute__((packed)) puntatore_alla_idt;

// Numero totale di voci nella IDT: fissato a 256 dal processore, non
// e' un valore personalizzabile.
#define NUMERO_DI_VOCI_NELLA_IDT 256

void idt_inizializza();
void idt_imposta_voce(int numero_interruzione, unsigned int indirizzo_funzione, unsigned short selettore_segmento, unsigned char attributi);

#endif