#include "kernel.h"
#include "delay.h"
#include "idt.h"
#include "mappa_memoria.h"
#include "stdf.h"



void kernel_main() <%

    idt_inizializza();
    temporizzatore_inizializza();
    mappa_memoria_inizializza();

    waaait(200);

    present();

    stampa_regioni_utilizzabili();

    print('\n');
    

    void *idx = alloc(654336);

    char *str;
    n_to_hex_str((int)idx,str);

    stampa_stringa(str);

    print('\n');

    idx = alloc(6);

    
    n_to_hex_str((int)idx,str);

    stampa_stringa(str);

    print('\n');

    while(1) {
        __asm__ volatile ("hlt");
    }
%>
