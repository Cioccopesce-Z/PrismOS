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

    while(1) {
        __asm__ volatile ("hlt");
    }
%>
