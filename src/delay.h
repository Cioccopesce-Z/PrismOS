// file: src/temporizzatore.h
#ifndef DELAY_H
#define DELAY_H

// Prepara il PIT (Programmable Interval Timer, il chip che genera
// un'interruzione hardware a intervalli regolari) e attiva la sua
// interruzione. Va chiamata DOPO idt_inizializza(): e' idt_inizializza()
// che, tra le altre cose, imposta anche la voce della IDT per questa
// interruzione (vedi sotto). Se il PIT venisse acceso prima che quella
// voce esista, la prima interruzione del timer troverebbe una voce
// vuota nella IDT: e' lo stesso identico problema che hai gia'
// risolto con la tastiera (il "kill yourself" seguito dal reset).
void temporizzatore_inizializza();

// Blocca l'esecuzione per almeno il numero di millisecondi indicato,
// senza tenere il processore occupato al 100% per niente: usa "hlt"
// per fermarlo fino alla prossima interruzione qualsiasi, invece di
// un ciclo vuoto che lo farebbe girare a vuoto per tutta l'attesa.
void temporizzatore_aspetta_millisecondi(unsigned int millisecondi);

// Gestore invocato automaticamente dal processore quando il PIT genera
// un'interruzione (circa 1000 volte al secondo, con la configurazione
// scelta qui sotto). Non va mai chiamata a mano dal resto del codice,
// esattamente come gestore_interruzione_tastiera.
__attribute__((interrupt))
void gestore_interruzione_temporizzatore(void *informazioni_interruzione);

#endif