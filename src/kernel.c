#include "kernel.h"

#include "delay.h"
#include "idt.h"
#include "loc.h"
#include "mem.h"
#include "mappa_memoria.h"
#include "stdf.h"

/* memoria vera e propria: allocata da begin(), dichiarata qui perche'
   e' il file-main che decide dimensioni e vive per tutta l'esecuzione. */
__uint8_t *memory;
size_t memory_cursor = 0;

/* configurazione della struttura: modificabile qui prima di begin(). */
size_t byte_for_lenght_of_the_lenght_of_the_scope = 1;
size_t byte_for_scope_code = 1;

size_t byte_for_scope = 0;
size_t byte_for_dim = 1;
size_t byte_for_vleng = 1;
size_t byte_for_method_lenght = 1;

void kernel_main() <%

    clear_screen(' ');

    idt_inizializza();
    temporizzatore_inizializza();
    mappa_memoria_inizializza();
    begin_fegh(30);

    waaait(200);

    present();

    stampa_regioni_utilizzabili();

    print('\n');
    

    unsigned char *idx;
    idx = alloc(8);

    fprint(idx,'p');
    print('\n');
    fprint(&idx[1],'p');
    print('\n');
    fprint(&idx[2],'p');
    print('\n');
    fprint(&idx[3],'p');

    stampa_stringa("\n\n");

    int ix = initialize_variable(tru, main_scope, fal, auto, 
        auto, 28, tru, 59);

    // ix e' un offset dentro il buffer "memory", non un indirizzo assoluto:
    // per ottenere un puntatore reale va sommato al puntatore base del
    // buffer stesso, non convertito da solo con itop().
    void *pt = memory + ix;

    int lesbian = get_value_of_variable(ix);

    fprint( &lesbian, 'i');
    print('\n');

    stampa_dump_memoria(memory,8);


    while(1) {
        __asm__ volatile ("hlt");
    }
%>
