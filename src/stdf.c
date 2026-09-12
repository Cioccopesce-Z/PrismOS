#include "kernel.h"

#include <stdarg.h>
#include "stdf.h"
#include "delay.h"
#include "type_conv.h"

static int riga_corrente = 0;
static int colonna_corrente = 0;


void input(char *output_pt)
{

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

// Muove il cursore di una posizione indietro e cancella il carattere
// che ci stava, per l'effetto visivo del backspace. E' l'UNICA
// funzione che un altro file deve chiamare per questo scopo: nessuno,
// al di fuori di questo file, deve mai leggere o scrivere
// riga_corrente/colonna_corrente direttamente, altrimenti si
// ripresenta lo stesso identico bug con un altro nome.
void sposta_cursore_indietro(void)
{
    if(colonna_corrente > 0)
    {
        colonna_corrente = colonna_corrente - 1;
    }
    else if(riga_corrente > 0)
    {
        // Il cursore era gia' a inizio riga: senza questo caso,
        // premere backspace li' sembrerebbe non fare nulla, mentre
        // in realta' dovrebbe tornare in fondo alla riga precedente.
        riga_corrente = riga_corrente - 1;
        colonna_corrente = SCREEN_WIDTH_IN_CHARACTERS - 1;
    }

    int indice_casella = riga_corrente * SCREEN_WIDTH_IN_CHARACTERS + colonna_corrente;
    scrivi_carattere_in_cella_schermo(indice_casella, ' ', DEFAULT_COLOR_ATTRIBUTE);
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





// Stampa in esadecimale, un byte alla volta, il contenuto di un
// intervallo di memoria: indirizzo del byte seguito dal suo valore.
// Serve per ispezionare a occhio cosa contiene davvero una zona di
// RAM quando non è disponibile un debugger: senza uno strumento del
// genere bisognerebbe indovinare il contenuto leggendo il codice,
// invece di vederlo direttamente.
void stampa_dump_memoria(unsigned char *indirizzo_di_partenza, unsigned int numero_di_byte)
{
    for (unsigned int indice_byte = 0; indice_byte < numero_di_byte; indice_byte++)
    {
        fprint("%p %x %d\n",
                         indirizzo_di_partenza + indice_byte,
                         (unsigned int)indirizzo_di_partenza[indice_byte],
                         (int)indirizzo_di_partenza[indice_byte]);
    }
}

// Stampa una stringa di formato sostituendo via via ogni specificatore
// (%d, %x, %s, %c, %%) con l'argomento corrispondente preso dalla lista variabile
void fprint(const char *formato, ...)
{
    va_list argomenti;
    va_start(argomenti, formato);   // si posiziona subito dopo "formato"

    int indice = 0;
    while (formato[indice] != '\0')
    {
        char carattere_corrente = formato[indice];

        if (carattere_corrente != '%')
        {
            print(carattere_corrente);
            indice++;
            continue;
        }

        indice++; // salta il '%', ora guardiamo lo specificatore
        char specificatore = formato[indice];

        switch (specificatore)
        {
            case 'd':
            {
                int valore = va_arg(argomenti, int);
                char testo_numero[21];

                if (valore < 0)
                {
                    print('-');
                    // Nota: qui non gestiamo il caso limite in cui
                    // valore e' esattamente il piu' piccolo int
                    // rappresentabile, dove "-valore" andrebbe in
                    // overflow
                    n_to_str((unsigned long long)(-valore), testo_numero);
                }
                else
                {
                    n_to_str((unsigned long long)valore, testo_numero);
                }

                stampa_stringa(testo_numero);
                break;
            }

            case 'x':
            {
                unsigned int valore = va_arg(argomenti, unsigned int);
                char testo_numero[19];
                n_to_hex_str((unsigned long long)valore, testo_numero);
                stampa_stringa(testo_numero);
                break;
            }

            case 's':
            {
                char *testo = va_arg(argomenti, char*);
                stampa_stringa(testo);
                break;
            }

            case 'c':
            {
                // char viene promosso a int nel passaggio variadico:
                // va letto come int, poi ridotto a char per stamparlo.
                char valore = (char) va_arg(argomenti, int);
                print(valore);
                break;
            }

            case '%':
            {
                print('%');
                break;
            }

            case 'p':
            {
                void *puntatore = va_arg(argomenti, void*);
                char testo_indirizzo[19];
                n_to_hex_str((unsigned long long)(unsigned long)puntatore, testo_indirizzo);
                stampa_stringa(testo_indirizzo);
                break;
            }

            default:
            {
                // Specificatore sconosciuto
                print('%');
                print(specificatore);
                break;
            }
        }

        indice++;
    }

    va_end(argomenti); // pulizia, obbligatoria per ogni va_start
}

void exit(int ext_code)
{
    exit_code = ext_code;
    kernel_main(true);
}