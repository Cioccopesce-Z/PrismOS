#include "loc.h"
#include "mappa_memoria.h"
#include "stdf.h"

// Stampa, una per riga, tutte le regioni di memoria utilizzabili
// trovate da mappa_memoria_inizializza(): indirizzo di partenza e
// lunghezza, entrambi in esadecimale.
void stampa_regioni_utilizzabili(void)
{
    char testo_indirizzo[19];
    char testo_lunghezza[19];

    unsigned int numero_di_regioni = mappa_memoria_numero_regioni_utilizzabili();

    for(unsigned int indice = 0; indice < numero_di_regioni; indice++)
    {
        struct regione_memoria_utilizzabile *regione = mappa_memoria_ottieni_regione(indice);

        n_to_hex_str(regione->indirizzo_base, testo_indirizzo);
        n_to_hex_str(regione->lunghezza_byte, testo_lunghezza);

        stampa_stringa(testo_indirizzo);
        print(' ');
        stampa_stringa(testo_lunghezza);
        print('\n');
    }
}



// Tiene traccia di ogni allocazione ancora attiva: indirizzo restituito
// al chiamante e quanti byte occupa. Serve a libera(), che riceve solo
// il puntatore (come free() in C standard) e altrimenti non saprebbe
// quanto spazio restituire alla mappa delle regioni libere.
struct blocco_allocato
{
    unsigned long long indirizzo_base;
    unsigned long long lunghezza_byte;
};

static struct blocco_allocato blocchi_allocati[NUMERO_MASSIMO_BLOCCHI_ALLOCATI];
static unsigned int numero_blocchi_allocati = 0;


// Alloca 'byte' byte di memoria, restituendo un indirizzo allineato a
// ALLINEAMENTO_ALLOCAZIONE byte. Strategia first-fit: scorre le regioni
// libere e usa la prima abbastanza grande da contenere sia il padding
// di allineamento sia i byte richiesti.
// Restituisce 0 (NULL) se non c'e' nessuna regione libera sufficiente.
void *alloc(unsigned long long byte)
{
    if (byte == 0)
    {
        return 0;
    }

    unsigned int numero_di_regioni = mappa_memoria_numero_regioni_utilizzabili();

    for (unsigned int indice = 0; indice < numero_di_regioni; indice++)
    {
        struct regione_memoria_utilizzabile *regione = mappa_memoria_ottieni_regione(indice);

        // Arrotonda l'indirizzo di partenza al prossimo multiplo di
        // ALLINEAMENTO_ALLOCAZIONE. Non possiamo assumere che
        // regione->indirizzo_base sia gia' allineato: ne' il BIOS ne'
        // la dimensione del kernel ce lo garantiscono.
        unsigned long long indirizzo_allineato =
            (regione->indirizzo_base + (ALLINEAMENTO_ALLOCAZIONE - 1))
            & ~(ALLINEAMENTO_ALLOCAZIONE - 1);

        // Byte "persi" tra l'inizio reale della regione e l'indirizzo
        // allineato: vanno comunque sottratti, altrimenti il prossimo
        // alloc() li considererebbe ancora liberi mentre li abbiamo
        // gia' scavalcati.
        unsigned long long spreco_padding = indirizzo_allineato - regione->indirizzo_base;

        if (regione->lunghezza_byte < spreco_padding + byte)
        {
            continue; // questa regione non basta, prova la prossima
        }

        regione->indirizzo_base = indirizzo_allineato + byte;
        regione->lunghezza_byte -= (spreco_padding + byte);

        if (numero_blocchi_allocati < NUMERO_MASSIMO_BLOCCHI_ALLOCATI)
        {
            blocchi_allocati[numero_blocchi_allocati].indirizzo_base = indirizzo_allineato;
            blocchi_allocati[numero_blocchi_allocati].lunghezza_byte = byte;
            numero_blocchi_allocati++;
        }

        return (void*)(unsigned long)indirizzo_allineato;
    }

    return 0; // nessuna regione libera abbastanza grande
}


// Cerca tra le regioni libere un vicino sinistro (che finisce esattamente
// dove inizia il blocco rilasciato) e uno destro (che inizia esattamente
// dove il blocco finisce), fondendo con entrambi se presenti invece di
// fermarsi al primo trovato.
// Restituisce 1 se ha fuso con almeno un vicino, 0 se il blocco resta
// isolato e va aggiunto come regione a se stante.
static int fondi_con_regione_adiacente(unsigned long long indirizzo_base, unsigned long long lunghezza_byte)
{
    unsigned long long fine_blocco = indirizzo_base + lunghezza_byte;

    int indice_vicino_sinistro = -1;
    int indice_vicino_destro = -1;

    // Primo giro: solo ricerca, nessuna modifica all'array. Farlo in un
    // unico passaggio senza toccare nulla evita che rimuovere una regione
    // a meta' ciclo sposti gli indici di quelle ancora da esaminare.
    unsigned int numero_di_regioni = mappa_memoria_numero_regioni_utilizzabili();
    for (unsigned int indice = 0; indice < numero_di_regioni; indice++)
    {
        struct regione_memoria_utilizzabile *regione = mappa_memoria_ottieni_regione(indice);
        unsigned long long fine_regione = regione->indirizzo_base + regione->lunghezza_byte;

        if (fine_regione == indirizzo_base)
        {
            indice_vicino_sinistro = (int)indice;
        }
        else if (regione->indirizzo_base == fine_blocco)
        {
            indice_vicino_destro = (int)indice;
        }
    }

    if (indice_vicino_sinistro == -1 && indice_vicino_destro == -1)
    {
        return 0; // nessun vicino: il blocco resta isolato
    }

    if (indice_vicino_sinistro != -1 && indice_vicino_destro != -1)
    {
        // [sinistra][blocco][destra] -> un'unica regione. Allarghiamo
        // quella di sinistra fino a coprire anche quella di destra, poi
        // eliminiamo quella di destra perche' ormai ridondante.
        struct regione_memoria_utilizzabile *sinistra =
            mappa_memoria_ottieni_regione((unsigned int)indice_vicino_sinistro);
        struct regione_memoria_utilizzabile *destra =
            mappa_memoria_ottieni_regione((unsigned int)indice_vicino_destro);

        sinistra->lunghezza_byte += lunghezza_byte + destra->lunghezza_byte;

        mappa_memoria_rimuovi_regione((unsigned int)indice_vicino_destro);

        return 1;
    }

    if (indice_vicino_sinistro != -1)
    {
        struct regione_memoria_utilizzabile *sinistra =
            mappa_memoria_ottieni_regione((unsigned int)indice_vicino_sinistro);
        sinistra->lunghezza_byte += lunghezza_byte;
        return 1;
    }

    // resta solo il caso: vicino trovato a destra
    struct regione_memoria_utilizzabile *destra =
        mappa_memoria_ottieni_regione((unsigned int)indice_vicino_destro);
    destra->indirizzo_base = indirizzo_base;
    destra->lunghezza_byte += lunghezza_byte;
    return 1;
}


// Rilascia un blocco ottenuto da alloc(), rendendolo di nuovo disponibile.
// Equivalente di free(). Cerca 'puntatore' tra i blocchi allocati per
// recuperarne la lunghezza, poi prova a fonderlo con una regione libera
// adiacente; se non trova vicini, lo aggiunge come regione a se stante.
void libera(void *puntatore)
{
    if (puntatore == 0)
    {
        return; // libera(NULL) non fa nulla, come free(NULL)
    }

    unsigned long long indirizzo_da_liberare = (unsigned long long)(unsigned long)puntatore;

    for (unsigned int indice = 0; indice < numero_blocchi_allocati; indice++)
    {
        if (blocchi_allocati[indice].indirizzo_base != indirizzo_da_liberare)
        {
            continue;
        }

        unsigned long long lunghezza_da_liberare = blocchi_allocati[indice].lunghezza_byte;

        // Rimuove il blocco dall'array spostando indietro tutti quelli
        // successivi, cosi' l'array resta compatto senza buchi
        for (unsigned int j = indice; j < numero_blocchi_allocati - 1; j++)
        {
            blocchi_allocati[j] = blocchi_allocati[j + 1];
        }
        numero_blocchi_allocati--;

        if (!fondi_con_regione_adiacente(indirizzo_da_liberare, lunghezza_da_liberare))
        {
            mappa_memoria_aggiungi_regione_libera(indirizzo_da_liberare, lunghezza_da_liberare);
        }

        return;
    }

    // puntatore non trovato: doppio libera(), o mai allocato da alloc().
    // Per ora ignorato silenziosamente; potresti stampare un errore qui
    // per scovare questi bug piu' facilmente durante lo sviluppo.
}

