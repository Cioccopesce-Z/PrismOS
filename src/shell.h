// file: src/shell.h
#ifndef SHELL_H
#define SHELL_H

// Lunghezza massima di una riga di comando, terminatore incluso.
// Personalizzabile: aumentarla non richiede toccare nient'altro.
#define LUNGHEZZA_MASSIMA_COMANDO 128

// Chiamata dal gestore di interruzione della tastiera per ogni
// carattere premuto, al posto della stampa diretta che fa ora.
// Accumula il carattere nel buffer di riga; se e' Invio, segnala
// al ciclo principale che un comando e' pronto da eseguire.
void shell_gestisci_carattere(char carattere);

// Da chiamare una volta all'avvio, dopo aver inizializzato tastiera
// e timer. Azzera lo stato interno e stampa il primo prompt.
void shell_inizializza(void);

// Da chiamare nel ciclo principale del kernel al posto di un "hlt"
// isolato: se un comando e' pronto lo esegue, altrimenti non fa
// nulla. Sicuro da chiamare di continuo.
void shell_ciclo_principale(void);

#endif