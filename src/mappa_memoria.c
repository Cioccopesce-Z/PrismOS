#include "mappa_memoria.h"

// Simboli del linker script: il loro INDIRIZZO e' il dato utile
extern char inizio_kernel;
extern char fine_kernel;

static struct regione_memoria_utilizzabile regioni_utilizzabili[NUMERO_MASSIMO_REGIONI_UTILIZZABILI];
static unsigned int numero_regioni_utilizzabili = 0;

static void aggiungi_regione_utilizzabile(unsigned long long indirizzo_base, unsigned long long lunghezza_byte)
{
    if (lunghezza_byte == 0)
    {
        return;
    }

    if (numero_regioni_utilizzabili < NUMERO_MASSIMO_REGIONI_UTILIZZABILI)
    {
        regioni_utilizzabili[numero_regioni_utilizzabili].indirizzo_base = indirizzo_base;
        regioni_utilizzabili[numero_regioni_utilizzabili].lunghezza_byte = lunghezza_byte;
        numero_regioni_utilizzabili++;
    }
}

// Taglia via lo spazio occupato dal kernel da una regione libera
static void aggiungi_regione_sottraendo_kernel(unsigned long long indirizzo_base, unsigned long long lunghezza_byte)
{
    unsigned long long fine_regione = indirizzo_base + lunghezza_byte;
    unsigned long long inizio_k = (unsigned long long)(unsigned long)&inizio_kernel;
    unsigned long long fine_k = (unsigned long long)(unsigned long)&fine_kernel;

    if (fine_k <= indirizzo_base || inizio_k >= fine_regione)
    {
        aggiungi_regione_utilizzabile(indirizzo_base, lunghezza_byte);
        return;
    }

    if (inizio_k > indirizzo_base)
    {
        aggiungi_regione_utilizzabile(indirizzo_base, inizio_k - indirizzo_base);
    }

    if (fine_k < fine_regione)
    {
        aggiungi_regione_utilizzabile(fine_k, fine_regione - fine_k);
    }
}

void mappa_memoria_inizializza(void)
<%
    unsigned int numero_di_voci_bios = *(unsigned int*)(INDIRIZZO_MAPPA_MEMORIA_BIOS);

    struct voce_mappa_memoria_bios *voci_bios =
        (struct voce_mappa_memoria_bios*)(INDIRIZZO_MAPPA_MEMORIA_BIOS + sizeof(unsigned int));

    numero_regioni_utilizzabili = 0;

    for (unsigned int indice = 0; indice < numero_di_voci_bios; indice++)
    {
        if (voci_bios[indice].tipo_regione == TIPO_MEMORIA_LIBERA)
        {
            aggiungi_regione_sottraendo_kernel(voci_bios[indice].indirizzo_base, voci_bios[indice].lunghezza_byte);
        }
    }
%>

unsigned int mappa_memoria_numero_regioni_utilizzabili(void)
{
    return numero_regioni_utilizzabili;
}

struct regione_memoria_utilizzabile *mappa_memoria_ottieni_regione(unsigned int indice)
{
    return &regioni_utilizzabili[indice];
}