// file: src/type_conv.c
#include "type_conv.h"

// Ogni funzione qui e' un cast diretto del C, incapsulato per avere
// un nome esplicito invece di un cast anonimo sparso nel codice.
// Dove la conversione puo' perdere informazione (per differenza di
// dimensione tra i tipi), il commento lo segnala.

char itoc(int value)
{
    // int e' a 32 bit, char a 8: se value supera l'intervallo di un
    // char, i 24 bit piu' alti vengono scartati silenziosamente.
    return (char) value;
}



int ctoi(char c)
{
    // Se char e' con segno (dipende dal compilatore, su i686-elf-gcc
    // lo e'), il valore viene esteso col segno passando a int.
    return (int) c;
}

//converte letteralmente il valore di pt come un floa lo reinterpretapraticamente
int ptoi(void *pt)
{
    // Cast intermedio a unsigned long: un puntatore va sempre trattato
    // come valore senza segno, altrimenti un indirizzo alto (sopra i
    // 2 GB) verrebbe interpretato come negativo prima ancora di finire
    // dentro l'int di ritorno.
    return (int) (unsigned long) pt;
}

int pvtoi(void *pt)
{
    return *(int*)pt;
}

//converte letteralmente il valore di pt come un floa lo reinterpretapraticamente
char ptoc(void *pt)
{
    // Un indirizzo e' a 32 bit, char a 8: qui si scartano sempre 24
    // dei 32 bit dell'indirizzo, quindi il risultato ha senso solo se
    // ti interessano gli 8 bit piu' bassi (ad esempio un offset).
    return (char) (unsigned long) pt;
}

char pvtoc(void *pt)
{
    return *(char*)pt;
}


void *itop(int value)
{
    // Cast intermedio a unsigned int: su i686 int e puntatore sono
    // entrambi a 32 bit, quindi qui non si perde e non si guadagna
    // nessun bit (a differenza di ptoi, dove il puntatore doveva
    // restringersi). Il passaggio da int a unsigned int serve solo a
    // evitare che un valore negativo generi un avviso del compilatore
    // in fase di conversione a puntatore.
    return (void*) (unsigned int) value;
}

void *ctop(char c)
{
    // Passaggio in due tappe: prima char diventa int (con estensione
    // di segno, se char e' con segno su questo compilatore), poi da
    // int a puntatore come in itop. Un indirizzo ottenuto cosi' ha
    // senso solo se ti serve un offset piccolo (il valore massimo
    // rappresentabile da un char e' comunque limitato a 255 o -128/127).
    return (void*) (unsigned int) (int) c;
}