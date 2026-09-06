#ifndef MAPPA_MEMORIA_H
#define MAPPA_MEMORIA_H

#define INDIRIZZO_MAPPA_MEMORIA_BIOS 0x8000   // concordato con boot.asm

#define TIPO_MEMORIA_LIBERA            1
#define TIPO_MEMORIA_RISERVATA         2
#define TIPO_MEMORIA_ACPI_RECUPERABILE 3
#define TIPO_MEMORIA_ACPI_NVS          4
#define TIPO_MEMORIA_DIFETTOSA         5

// Formato imposto dal BIOS, non modificabile
struct voce_mappa_memoria_bios
{
    unsigned long long indirizzo_base;
    unsigned long long lunghezza_byte;
    unsigned int tipo_regione;
    unsigned int attributi_estesi;
} __attribute__((packed));

// Formato libero
struct regione_memoria_utilizzabile
{
    unsigned long long indirizzo_base;
    unsigned long long lunghezza_byte;
};

#define NUMERO_MASSIMO_REGIONI_UTILIZZABILI 32

// Restituisce spazio alla mappa delle regioni libere (usata da libera()
// in stdf.c per non dover accedere direttamente all'array interno,
// che resta privato a questo file).
void mappa_memoria_aggiungi_regione_libera(unsigned long long indirizzo_base, unsigned long long lunghezza_byte);

// Rimuove la regione all'indice dato, compattando l'array. Restituisce
// 1 se rimossa, 0 se l'indice non era valido. Serve a libera(), per il
// caso in cui un blocco rilasciato faccia da "ponte" tra due regioni
// libere gia' esistenti: una delle due, dopo la fusione, diventa
// ridondante e va eliminata.
int mappa_memoria_rimuovi_regione(unsigned int indice);

void mappa_memoria_inizializza(void);
unsigned int mappa_memoria_numero_regioni_utilizzabili(void);
struct regione_memoria_utilizzabile *mappa_memoria_ottieni_regione(unsigned int indice);

#endif