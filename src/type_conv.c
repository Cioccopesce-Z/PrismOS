/*// file: src/type_conv.c
#include "type_conv.h"

// Includere il proprio header anche nel file che lo implementa fa
// controllare al compilatore che ogni funzione qui sotto abbia
// esattamente lo stesso tipo di ritorno e di parametri dichiarati nel
// file .h. Senza questo controllo, un disallineamento tra i due file
// emergerebbe solo in fase di collegamento (link), con un errore
// molto meno chiaro da interpretare.


char itoc(int value)
{
    // Tiene solo gli 8 bit piu' bassi del valore: e' una perdita di
    // informazione voluta. Va usata solo quando sai gia' che 'value'
    // rientra nell'intervallo rappresentabile da un char.
    return (char)value;
}

float itof(int value)
{
    // Conversione vera: cambia la rappresentazione (da intero in
    // complemento a due a virgola mobile) ma preserva il valore
    // numerico, nei limiti di precisione spiegati piu' sotto per
    // ptof.
    return (float)value;
}


int ctoi(char c)
{
    // Allargamento sicuro: un char entra sempre in un int senza
    // perdita di informazione, quindi qui non ci sono casi limite.
    return (int)c;
}

float ctof(char c)
{
    return (float)c;
}


int ftoi(float fvalue)
{
    // Tronca la parte decimale, non arrotonda: 3.9 diventa 3, non 4.
    // E' il comportamento standard del cast in C, non una scelta di
    // questo progetto.
    return (int)fvalue;
}

char ftoc(float fvalue)
{
    return (char)fvalue;
}


// ---------------------------------------------------------------------
// Da qui in poi: conversioni che partono da un puntatore. Un
// puntatore, a basso livello, e' semplicemente un numero senza segno
// che rappresenta un indirizzo di memoria. Le funzioni seguenti
// prendono QUEL numero e lo reinterpretano come intero o come
// virgola mobile: non vanno a leggere il contenuto della memoria a
// quell'indirizzo. Se in futuro ti serve leggere il contenuto, serve
// la dereferenziazione, per esempio "int valore = *(int*)puntatore;",
// che e' un'operazione diversa e non appartiene a questo file.
// ---------------------------------------------------------------------

int ptoi(void *pt)
{
    // Passaggio intermedio per "unsigned long": su questo
    // compilatore, per un bersaglio a 32 bit come il tuo, sia i
    // puntatori sia "unsigned long" occupano esattamente 32 bit,
    // quindi questo primo passaggio preserva l'indirizzo esatto, bit
    // per bit, senza segno.
    //
    // Il secondo passaggio, verso "int" (che ha il segno), non perde
    // bit ma ne cambia l'interpretazione: se l'indirizzo e'
    // 0x80000000 o superiore (2 GB in su), il bit piu' alto viene
    // letto come segno e il numero appare negativo, pur essendo
    // esattamente lo stesso pattern di bit dell'indirizzo di
    // partenza. Non e' correggibile dentro questa funzione: e' un
    // limite intrinseco di "int" con segno usato per un valore che
    // concettualmente non ha segno. Se in futuro ti serve confrontare
    // o stampare indirizzi che possono superare i 2 GB, non passare
    // da ptoi: usa direttamente "(unsigned long)puntatore" nel punto
    // dove ti serve, come gia' fai in mappa_memoria.c e stdf.c.
    return (int)(unsigned long)pt;
}

char ptoc(void *pt)
{
    // Attenzione: un indirizzo occupa 32 bit, un char ne occupa 8.
    // Questa conversione tiene solo gli 8 bit piu' bassi
    // dell'indirizzo e scarta silenziosamente gli altri 24: due
    // indirizzi diversi possono produrre lo stesso risultato, quindi
    // il valore restituito non identifica in alcun modo l'indirizzo
    // originale. Lasciata per compatibilita' con il resto del
    // codice, ma quasi certamente non e' quello che vuoi se ti serve
    // davvero un indirizzo: se invece ti serve il byte puntato da
    // 'pt' (il contenuto della memoria a quella posizione, non
    // l'indirizzo stesso), quella e' una dereferenziazione:
    // "char valore = *(char*)pt;", non questa funzione.
    return (char)(unsigned long)pt;
}

float ptof(void *pt)
{
    // Eredita il limite di ptoi (segno perso per indirizzi oltre i
    // 2 GB), e ne aggiunge uno nuovo, specifico del float: un float a
    // precisione singola (formato IEEE754 a 32 bit) ha solo 24 bit di
    // mantissa utilizzabili per rappresentare un intero in modo
    // esatto, cioe' puo' farlo solo fino a circa 16,7 milioni. Un
    // indirizzo a 32 bit puo' superare i 4 miliardi: oltre quella
    // soglia il float arrotonda al valore rappresentabile piu'
    // vicino, perdendo precisione. Va bene per indirizzi piccoli
    // (i primi megabyte di RAM, dove probabilmente gira il tuo
    // kernel adesso), ma tienilo a mente se in futuro lavori con
    // indirizzi piu' alti.
    return itof(ptoi(pt));
}
    */