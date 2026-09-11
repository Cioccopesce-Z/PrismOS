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
    char temp = value;
    return temp;
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

// Converte un numero a 64 bit nella sua rappresentazione esadecimale,
// con prefisso "0x", scrivendola in buffer_destinazione. Il buffer
// deve avere spazio per almeno 19 caratteri: "0x" + 16 cifre + '\0'.
//
// L'esadecimale si presta a essere costruito dalla cifra piu'
// significativa in poi (a differenza del decimale, vedi sotto),
// perche' ogni cifra corrisponde esattamente a 4 bit: possiamo
// "affettare" il numero a blocchi di 4 bit partendo da sinistra,
// senza dover prima contare quante cifre avra' il risultato.
void n_to_hex_str(unsigned long long valore, char *buffer_destinazione)
{
    const char cifre_esadecimali[] = "0123456789ABCDEF";
    int posizione_scrittura = 0;

    buffer_destinazione[posizione_scrittura++] = '0';
    buffer_destinazione[posizione_scrittura++] = 'x';

    for(int indice_cifra = 15; indice_cifra >= 0; indice_cifra--)
    {
        unsigned int spostamento_bit = indice_cifra * 4;
        unsigned int valore_cifra = (valore >> spostamento_bit) & 0xF;
        buffer_destinazione[posizione_scrittura++] = cifre_esadecimali[valore_cifra];
    }

    buffer_destinazione[posizione_scrittura] = '\0';
}

// Converte un numero a 64 bit nella sua rappresentazione decimale,
// scrivendola in buffer_destinazione. Il buffer deve avere spazio
// per almeno 21 caratteri (fino a 20 cifre per un numero a 64 bit,
// piu' il terminatore).
//
// A differenza dell'esadecimale, dividere per 10 estrae le cifre
// partendo dalla MENO significativa (l'ultima cifra del numero, non
// la prima). Per questo le cifre vengono prima raccolte in ordine
// inverso, poi ricopiate nell'ordine giusto: e' il modo standard di
// scrivere questa conversione, vale la pena capirlo bene una volta.
void n_to_str(unsigned long long valore, char *buffer_destinazione)
{
    char cifre_in_ordine_inverso[21];
    int numero_di_cifre = 0;

    if(valore == 0)
    {
        buffer_destinazione[0] = '0';
        buffer_destinazione[1] = '\0';
        return;
    }

    while(valore > 0)
    {
        cifre_in_ordine_inverso[numero_di_cifre] = '0' + (valore % 10);
        valore = valore / 10;
        numero_di_cifre++;
    }

    for(int indice = 0; indice < numero_di_cifre; indice++)
    {
        buffer_destinazione[indice] = cifre_in_ordine_inverso[numero_di_cifre - 1 - indice];
    }
    buffer_destinazione[numero_di_cifre] = '\0';
}

// Converte una stringa esadecimale (facoltativamente con prefisso "0x"
// o "0X", come quella prodotta da n_to_hex_str) nel numero intero che
// rappresenta. La conversione si ferma al primo carattere che non e'
// una cifra esadecimale valida, oppure alla fine della stringa
// ('\0'), quel che viene prima.
int hex_str_to_n(char str[])
{
    int indice_carattere = 0;
    int valore_risultante = 0;

    // Salta il prefisso "0x" o "0X", se presente. Non deve essere
    // obbligatorio: la funzione deve poter convertire anche stringhe
    // esadecimali scritte senza prefisso.
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        indice_carattere = 2;
    }

    while (str[indice_carattere] != '\0')
    {
        char carattere_corrente = str[indice_carattere];
        int valore_della_cifra;

        if (carattere_corrente >= '0' && carattere_corrente <= '9')
        {
            valore_della_cifra = carattere_corrente - '0';
        }
        else if (carattere_corrente >= 'A' && carattere_corrente <= 'F')
        {
            valore_della_cifra = (carattere_corrente - 'A') + 10;
        }
        else if (carattere_corrente >= 'a' && carattere_corrente <= 'f')
        {
            valore_della_cifra = (carattere_corrente - 'a') + 10;
        }
        else
        {
            // Carattere non esadecimale: la conversione finisce qui,
            // esattamente come fa la atoi() dello standard C quando
            // incontra un carattere non numerico.
            break;
        }

        // Ogni cifra esadecimale in piu' sposta cio' che avevamo gia'
        // accumulato di 4 bit verso sinistra (moltiplicare per 16
        // equivale a scorrere di 4 bit, essendo 16 = 2^4), poi
        // aggiunge la nuova cifra nei 4 bit piu' bassi appena
        // liberati.
        valore_risultante = (valore_risultante << 4) | valore_della_cifra;

        indice_carattere++;
    }

    return valore_risultante;
}