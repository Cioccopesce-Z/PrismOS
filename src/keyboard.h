// file: src/keyboard.h
#ifndef KEYBOARD_H
#define KEYBOARD_H

// Funzione eseguita automaticamente dal processore ogni volta che
// arriva un'interruzione dalla tastiera. L'attributo "interrupt" fa
// generare a GCC il codice di salvataggio/ripristino dei registri e
// l'uso dell'istruzione IRET al posto della normale RET, che altrimenti
// andrebbero scritti a mano in assembly.
__attribute__((interrupt))
void gestore_interruzione_tastiera(void *informazioni_interruzione);

#endif