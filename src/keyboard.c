// file: src/keyboard.c
#include "keyboard.h"
#include "kernel.h"
#include "io.h"

// Fa corrispondere ogni "scan code" (il numero che la tastiera invia
// per identificare fisicamente quale tasto e' stato premuto, in base
// alla posizione) al carattere corrispondente. Questa tabella assume
// una disposizione dei tasti in stile QWERTY statunitense: le lettere
// funzioneranno comunque, ma se la tua tastiera fisica e' italiana,
// alcuni simboli potrebbero non corrispondere a quello stampato sul
// tasto. Estendere questa tabella per un layout italiano completo e'
// possibile in futuro, aggiungendo semplicemente altre voci qui sotto.
static const char tabella_scan_code_in_caratteri[59] = {
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4', [0x06] = '5',
    [0x07] = '6', [0x08] = '7', [0x09] = '8', [0x0A] = '9', [0x0B] = '0',
    [0x10] = 'q', [0x11] = 'w', [0x12] = 'e', [0x13] = 'r', [0x14] = 't',
    [0x15] = 'y', [0x16] = 'u', [0x17] = 'i', [0x18] = 'o', [0x19] = 'p',
    [0x1E] = 'a', [0x1F] = 's', [0x20] = 'd', [0x21] = 'f', [0x22] = 'g',
    [0x23] = 'h', [0x24] = 'j', [0x25] = 'k', [0x26] = 'l',
    [0x2C] = 'z', [0x2D] = 'x', [0x2E] = 'c', [0x2F] = 'v', [0x30] = 'b',
    [0x31] = 'n', [0x32] = 'm',
    [0x1C] = '\n', // tasto Invio
    [0x39] = ' ',  // barra spaziatrice
};

__attribute__((interrupt))
void gestore_interruzione_tastiera(void *informazioni_interruzione){
    // Legge dalla porta 0x60 il byte che la tastiera ha appena inviato.
    // E' la porta standard da cui si leggono i dati della tastiera,
    // fissata dall'hardware, non personalizzabile.
    unsigned char scan_code = leggi_byte_da_porta(0x60);

    // La tastiera invia un "make code" quando un tasto viene premuto,
    // e lo stesso numero con il bit piu' alto acceso (sommato a 0x80,
    // cioe' un "break code") quando viene rilasciato. Per ora
    // interessano solo le pressioni: i rilasci vengono ignorati.
    if(scan_code < 0x80){
        if(scan_code < sizeof(tabella_scan_code_in_caratteri) && tabella_scan_code_in_caratteri[scan_code] != 0){
            stampa_carattere(tabella_scan_code_in_caratteri[scan_code]);
        }
    }

    // Segnala al PIC che questa interruzione e' stata gestita. Senza
    // questo passaggio il PIC crede che il processore sia ancora
    // occupato con l'interruzione corrente e smette di inviarne altre:
    // la tastiera sembrerebbe funzionare una volta sola e poi bloccarsi.
    scrivi_byte_su_porta(0x20, 0x20);
}