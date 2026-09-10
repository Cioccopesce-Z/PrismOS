#include "kernel.h"
#include "stdf.h"
#include "delay.h"

// Stampa un valore di tipo variabile a seconda di 'tipo'. Per 's' il
// chiamante passa direttamente il puntatore alla stringa (una stringa
// in C e' gia' di per se' un indirizzo al suo primo carattere, non
// serve altro). Per tutti gli altri tipi, il chiamante deve passare
// l'INDIRIZZO della variabile (con l'operatore &), perche' qui dentro
// serve leggere il valore vero e proprio andando a quell'indirizzo,
// non semplicemente reinterpretare l'indirizzo stesso come se fosse
// il dato: sono due operazioni diverse.
//
// Esempi d'uso:
//   int numero = 42;
//   stampa_valore(&numero, 'i');
//
//   char lettera = 'A';
//   stampa_valore(&lettera, 'c');
//
//   stampa_valore("ciao", 's');
void fprint(void *dato, char tipo)
{
    switch (tipo)
    {
        case 's':

            stampa_stringa(dato);

        break;

        case 'i':
        {
            // Va dereferenziato: 'dato' e' l'indirizzo dell'intero,
            // non l'intero stesso. (int*)dato dice al compilatore
            // "qui c'e' un intero", e * legge quell'intero dalla
            // memoria a quell'indirizzo.
            int valore = *(int*)dato;
            char testo[21]; // spazio sufficiente per un numero a 64 bit piu' segno

            if (valore < 0)
            {
                print('-');
                // Nota: non gestisce il caso limite di INT_MIN, il
                // valore intero negativo piu' piccolo rappresentabile,
                // la cui negazione andrebbe in overflow. Caso raro,
                // lasciato volutamente non gestito per ora.
                n_to_str((unsigned long long)(-valore), testo);
            }
            else
            {
                n_to_str((unsigned long long)valore, testo);
            }

            stampa_stringa(testo);
        }
        break;

        case 'f':

            // Non ancora implementato: manca una funzione che converta
            // un float in testo, e usare float nel kernel richiede
            // prima di inizializzare correttamente la FPU (argomento
            // separato, da affrontare piu' avanti).

        break;

        case 'c':
        {
            char valore = *(char*)dato;
            print(valore);
        }
        break;

        case 'h':

            // Non ancora implementato: riservato per una futura
            // stampa in esadecimale di un valore intero (da
            // distinguere dal case 'p', che stampa l'indirizzo stesso).

        break;

        case 'p':
        {
            // Qui non serve dereferenziare: vogliamo stampare
            // l'indirizzo stesso contenuto in 'dato', non il valore
            // a cui punta.
            char testo_indirizzo[19]; // "0x" + 16 cifre + terminatore
            n_to_hex_str((unsigned long long)(unsigned long)dato, testo_indirizzo);
            stampa_stringa(testo_indirizzo);
        }
        break;

        default:

            // Tipo sconosciuto: ignorato silenziosamente.

        break;
    }
}


// Scrive un carattere e il suo colore in una cella dello schermo,
// identificata dal suo indice (0 = angolo in alto a sinistra,
// 1 = cella successiva, e cosi' via in orizzontale).
// E' l'unica funzione che scrive davvero in VIDEO_MEMORY_ADDRESS:
// clear_screen, stampa_carattere e print_at_color si appoggiano
// tutte a questa, invece di ripetere lo stesso accesso a memoria.
static void scrivi_carattere_in_cella_schermo(int indice_casella, char carattere, char colore)
{
    char *video_memory = (char*) VIDEO_MEMORY_ADDRESS;
    video_memory[indice_casella * 2] = carattere;
    video_memory[indice_casella * 2 + 1] = colore;
}


void clear_screen(char character_to_use_to_fill_the_screen)
{
    int numero_totale_di_caselle = SCREEN_WIDTH_IN_CHARACTERS * SCREEN_HEIGHT_IN_CHARACTERS;

    for(int indice_casella = 0; indice_casella < numero_totale_di_caselle; indice_casella++)
    {
        scrivi_carattere_in_cella_schermo(indice_casella, character_to_use_to_fill_the_screen, DEFAULT_COLOR_ATTRIBUTE);
    }

    riga_corrente = 0;
    colonna_corrente = 0;
}


// Scrive un carattere alla posizione attuale del cursore e sposta il
// cursore in avanti. Versione volutamente semplice: quando lo schermo
// si riempie, ricomincia dall'alto invece di far scorrere il contenuto
// verso l'alto (scorrimento vero, da implementare in seguito quando
// affronteremo la stampa di stringhe in modo completo).
void print(char carattere)
{
    if(carattere == '\n')
    {
        riga_corrente = riga_corrente + 1;
        colonna_corrente = 0;
    }
    else
    {
        int indice_casella = riga_corrente * SCREEN_WIDTH_IN_CHARACTERS + colonna_corrente;
        scrivi_carattere_in_cella_schermo(indice_casella, carattere, DEFAULT_COLOR_ATTRIBUTE);

        colonna_corrente = colonna_corrente + 1;
        if(colonna_corrente >= SCREEN_WIDTH_IN_CHARACTERS)
        {
            colonna_corrente = 0;
            riga_corrente = riga_corrente + 1;
        }
    }

    if(riga_corrente >= SCREEN_HEIGHT_IN_CHARACTERS)
    {
        riga_corrente = 0;
    }
}


// Stampa una stringa terminata da '\0' alla posizione attuale del
// cursore, riusando stampa_carattere un carattere alla volta. Serve
// per stampare testo composto da piu' caratteri (come i numeri
// convertiti in stringa qui sotto) senza scrivere lo stesso ciclo
// ogni volta che serve.
void stampa_stringa(char stringa[])
{
    int indice = 0;
    while(stringa[indice] != '\0')
    {
        print(stringa[indice]);
        indice++;
    }
}


void print_at_color(char *str, int row, int col, char color)
{
    int j = 0;

    while (str[j] != '\0')
    {
        int pos = row * SCREEN_WIDTH + col + j;
        scrivi_carattere_in_cella_schermo(pos, str[j], color);
        j++;
    }
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

void present(){
    prism_screen();

    waaait(700);

    clear_screen(' ');

    waaait(300);

    prism_screen();

    waaait(700);

    clear_screen(' ');

    waaait(300);

    prism_screen();

    waaait(700);

    clear_screen('m');
    waaait(30);
    clear_screen(' ');

    waaait(300);
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


