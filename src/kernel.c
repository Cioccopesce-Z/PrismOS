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

    fprint("%p\n%p\n%p\n%p\n\n",idx,&idx[1],&idx[2],&idx[3]);


    int ix = initialize_variable(tru, main_scope, fal, auto, 
        auto, 28, tru, 59);


    void *pt = memory + ix;
    
    fprint("%d\n",get_value_of_variable(ix));

    stampa_dump_memoria(memory,8);


    while(1) {
        __asm__ volatile ("hlt");
    }
%>
