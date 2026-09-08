#include "kernel.h"
#include "delay.h"
#include "idt.h"
#include "loc.h"
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
    

    unsigned char *idx;


    idx = alloc(654336);
    fprint(idx,'p');
    print('\n');



    idx = alloc(1);
    fprint(idx,'p');
    print('\n');



    idx = alloc(2);
    fprint(idx,'p');
    fprint("\n\n",'s');

    libera(idx);



    idx = alloc(8);

    fprint(idx,'p');
    print('\n');
    fprint(&idx[1],'p');
    print('\n');
    fprint(&idx[2],'p');
    print('\n');
    fprint(&idx[3],'p');
    print('\n');


    while(1) {
        __asm__ volatile ("hlt");
    }
%>
