// file: src/idt.c
#include "idt.h"
#include "io.h"
#include "keyboard.h"

// La tabella vera e propria. E' una variabile globale perche' deve
// restare in memoria per tutta la vita del kernel: il processore la
// consulta ogni volta che arriva un'interruzione, non solo adesso
// durante l'inizializzazione.
static voce_della_idt tabella_idt[NUMERO_DI_VOCI_NELLA_IDT];
static puntatore_alla_idt puntatore_idt;

void idt_imposta_voce(int numero_interruzione, unsigned int indirizzo_funzione, unsigned short selettore_segmento, unsigned char attributi){
    tabella_idt[numero_interruzione].indirizzo_gestore_parte_bassa = indirizzo_funzione & 0xFFFF;
    tabella_idt[numero_interruzione].indirizzo_gestore_parte_alta  = (indirizzo_funzione >> 16) & 0xFFFF;
    tabella_idt[numero_interruzione].selettore_segmento_codice = selettore_segmento;
    tabella_idt[numero_interruzione].riservato = 0;
    tabella_idt[numero_interruzione].attributi_e_tipo = attributi;
}

// Riconfigura il PIC spostando le interruzioni hardware (IRQ 0-7,
// tastiera compresa) dai numeri 0x08-0x0F, dove sono per default, ai
// numeri 0x20 in su. Il motivo: il processore usa gia' i numeri da
// 0x00 a 0x1F per le proprie eccezioni interne (divisione per zero,
// accesso a memoria non valido, eccetera), quindi senza questo
// spostamento le due cose si sovrapporrebbero. La sequenza di byte qui
// sotto e' un protocollo di inizializzazione fisso del chip, definito
// dal suo produttore: non e' personalizzabile, va seguita cosi' com'e'.
static void rimappa_il_pic(){
    scrivi_byte_su_porta(0x20, 0x11);
    scrivi_byte_su_porta(0xA0, 0x11);

    scrivi_byte_su_porta(0x21, 0x20); // PIC principale: interruzioni a partire da 0x20
    scrivi_byte_su_porta(0xA1, 0x28); // PIC secondario: interruzioni a partire da 0x28

    scrivi_byte_su_porta(0x21, 0x04);
    scrivi_byte_su_porta(0xA1, 0x02);

    scrivi_byte_su_porta(0x21, 0x01);
    scrivi_byte_su_porta(0xA1, 0x01);

    scrivi_byte_su_porta(0x21, 0x00); // abilita tutte le interruzioni del PIC principale
    scrivi_byte_su_porta(0xA1, 0x00); // abilita tutte le interruzioni del PIC secondario
}

void idt_inizializza(){
    // Azzera tutta la tabella: le interruzioni non configurate
    // esplicitamente restano vuote. Se una di queste dovesse comunque
    // verificarsi, il processore segnala un errore invece di eseguire
    // codice a caso da un indirizzo non inizializzato.
    for(int indice = 0; indice < NUMERO_DI_VOCI_NELLA_IDT; indice++){
        idt_imposta_voce(indice, 0, 0, 0);
    }

    rimappa_il_pic();

    // La tastiera e' collegata alla linea IRQ1, che dopo la
    // rimappatura corrisponde al numero di interruzione 0x21 (0x20 + 1).
    // Il valore 0x8E negli attributi significa: interruzione presente,
    // livello di privilegio 0 (kernel), tipo "interrupt gate" a 32 bit.
    // Il selettore 0x08 e' lo stesso CODE_OFFSET che trovi in boot.asm:
    // sono due file separati (assembly e C) che non condividono
    // automaticamente le costanti, quindi se in futuro cambi la GDT
    // in boot.asm devi ricordarti di aggiornare anche questo numero qui.
    idt_imposta_voce(0x21, (unsigned int) gestore_interruzione_tastiera, 0x08, 0x8E);

    puntatore_idt.dimensione_tabella_meno_uno = sizeof(tabella_idt) - 1;
    puntatore_idt.indirizzo_tabella = (unsigned int) &tabella_idt;

    __asm__ volatile ("lidt %0" : : "m"(puntatore_idt));

    // Abilita le interruzioni. Va fatto solo ora, alla fine: se le
    // avessimo abilitate prima che la tabella fosse pronta, una
    // interruzione arrivata in quel momento avrebbe trovato una
    // tabella vuota o incompleta.
    __asm__ volatile ("sti");
}