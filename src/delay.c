// file: src/temporizzatore.c
#include "delay.h"
#include "io.h"
#include "idt.h"

// Frequenza naturale del cristallo che alimenta il PIT: circa 1,193182
// milioni di volte al secondo. E' un valore storico ereditato dal
// primo IBM PC del 1981, rimasto lo standard su ogni PC compatibile
// (QEMU compreso) da allora. E' un dato di fatto imposto dall'hardware:
// non e' personalizzabile e non serve impararlo a memoria, basta
// sapere che esiste e viene usato nel calcolo del divisore qui sotto.
#define FREQUENZA_BASE_DEL_PIT 1193182

// Quante volte al secondo vogliamo che il timer generi un'interruzione.
// QUESTO E' PERSONALIZZABILE: un valore piu' alto da' un delay piu'
// preciso ma genera piu' interruzioni (piu' lavoro per il processore);
// un valore piu' basso e' piu' leggero ma meno preciso. Ho scelto 1000
// apposta: cosi' un singolo "tick" del timer corrisponde esattamente a
// un millisecondo, ed evita moltiplicazioni o divisioni quando si
// calcola quanto aspettare.
#define FREQUENZA_INTERRUZIONI_TIMER 1000

// Incrementato di uno a ogni interruzione del timer, quindi mille
// volte al secondo. E' "volatile" perche' viene modificato dentro un
// gestore di interruzione, che dal punto di vista del compilatore puo'
// scattare in un punto qualsiasi del programma: senza "volatile" il
// compilatore potrebbe pensare che, dentro un ciclo che lo legge piu'
// volte, il suo valore non possa cambiare da solo, tenerlo in un
// registro invece di rileggerlo dalla memoria, e il ciclo non
// finirebbe mai.
static volatile unsigned int contatore_millisecondi = 0;

__attribute__((interrupt))
void gestore_interruzione_temporizzatore(void *informazioni_interruzione){
    contatore_millisecondi = contatore_millisecondi + 1;

    // Segnala al PIC che l'interruzione e' stata gestita, esattamente
    // come gia' fa gestore_interruzione_tastiera. Senza questo, il PIC
    // crede che il processore sia ancora occupato con questa
    // interruzione e smette di mandarne altre.
    scrivi_byte_su_porta(0x20, 0x20);
}

void temporizzatore_inizializza(){
    // Registra questo gestore alla voce 0x20 della IDT. Dopo la
    // rimappatura del PIC fatta in idt.c, l'IRQ0 (la linea a cui e'
    // collegato il PIT) corrisponde proprio al numero 0x20, essendo la
    // prima linea del PIC principale (0x20 + 0). Il selettore 0x08 e
    // il tipo 0x8E hanno lo stesso identico significato gia' visto per
    // la tastiera in idt.c.
    //
    // Nota su come funziona questa riga: idt_inizializza() ha gia'
    // eseguito "lidt" (che dice al processore dove si trova la
    // tabella), ma questo NON e' un problema: "lidt" comunica solo
    // l'indirizzo e la dimensione della tabella, non ne fa una copia.
    // Il processore rilegge la tabella direttamente dalla memoria a
    // ogni interruzione, quindi modificarla ora, dopo, e' sicuro:
    // la modifica e' visibile immediatamente.
    idt_imposta_voce(0x20, (unsigned int) gestore_interruzione_temporizzatore, 0x08, 0x8E);

    // Il PIT non si programma dicendogli direttamente una frequenza in
    // Hz, ma dicendogli ogni quanti colpi del suo cristallo interno
    // deve generare un'interruzione.
    unsigned int divisore = FREQUENZA_BASE_DEL_PIT / FREQUENZA_INTERRUZIONI_TIMER;

    // Byte di comando per il PIT (chip Intel 8253/8254). Il FORMATO
    // dei bit e' imposto dal chip, non personalizzabile liberamente;
    // la SCELTA di quale modalita' tra quelle disponibili usare, si'.
    // Scomponendo 0x36 = 0011 0110:
    //   bit 7-6 = 00  -> canale 0 (quello collegato all'IRQ0)
    //   bit 5-4 = 11  -> il divisore va inviato in due passaggi:
    //                    prima il byte basso, poi quello alto
    //   bit 3-1 = 011 -> modalita' 3, "generatore di onda quadra":
    //                    genera interruzioni a intervalli regolari e
    //                    ricomincia da solo, all'infinito
    //   bit 0   = 0   -> numeri in binario normale, non BCD
    // Questa combinazione e' lo standard di fatto per un'interruzione
    // periodica. Vale la pena sapere CHE COSA rappresentano questi
    // bit, non serve memorizzare il valore 0x36 in se'.
    unsigned char byte_di_comando = 0x36;
    scrivi_byte_su_porta(0x43, byte_di_comando); // 0x43 = porta comando del PIT, fissa dall'hardware

    // Il divisore e' un numero a 16 bit, ma le porte del PIT accettano
    // un byte alla volta: va spedito prima il byte basso, poi quello
    // alto, nell'ordine imposto dai bit 5-4 scelti sopra.
    scrivi_byte_su_porta(0x40, divisore & 0xFF);        // byte basso
    scrivi_byte_su_porta(0x40, (divisore >> 8) & 0xFF); // byte alto

    // L'IRQ0 nasce mascherata (disabilitata) dalla rimappatura del PIC
    // fatta in idt.c, che abilita esplicitamente solo l'IRQ1
    // (tastiera, maschera 0xFD). Qui la sblocchiamo senza toccare lo
    // stato delle altre linee: leggiamo la maschera attuale, spegniamo
    // solo il bit dell'IRQ0 (bit 0) e riscriviamo il risultato.
    // Farlo cosi', invece di scrivere un valore fisso nuovo, evita che
    // questo file e idt.c debbano restare sincronizzati a mano su
    // quali IRQ sono abilitate: ognuno gestisce la propria.
    unsigned char maschera_attuale = leggi_byte_da_porta(0x21);
    unsigned char nuova_maschera = maschera_attuale & 0xFE; // 0xFE = 1111 1110, spegne solo il bit 0
    scrivi_byte_su_porta(0x21, nuova_maschera);
}

void temporizzatore_aspetta_millisecondi(unsigned int millisecondi){
    unsigned int millisecondo_di_arrivo = contatore_millisecondi + millisecondi;

    while(contatore_millisecondi < millisecondo_di_arrivo){
        // Ferma il processore finche' non arriva la prossima
        // interruzione qualsiasi (quella del timer, o anche quella
        // della tastiera se nel frattempo premi un tasto): appena si
        // risveglia, il ciclo ricontrolla la condizione. Molto piu'
        // economico di un ciclo vuoto che terrebbe il processore al
        // massimo per tutta l'attesa.
        __asm__ volatile ("hlt");
    }
}