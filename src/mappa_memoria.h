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

// Formato nostro, libero
struct regione_memoria_utilizzabile
{
    unsigned long long indirizzo_base;
    unsigned long long lunghezza_byte;
};

#define NUMERO_MASSIMO_REGIONI_UTILIZZABILI 32

void mappa_memoria_inizializza(void);
unsigned int mappa_memoria_numero_regioni_utilizzabili(void);
struct regione_memoria_utilizzabile *mappa_memoria_ottieni_regione(unsigned int indice);

#endif