#include "kernel.h"
#include "delay.h"
#include "idt.h"
#include "mappa_memoria.h"
#include "stdf.h"



void kernel_main() <%

    clear_screen(' ');
    idt_inizializza();
    temporizzatore_inizializza();
    mappa_memoria_inizializza();

    prism_screen();

    temporizzatore_aspetta_millisecondi(700);

    clear_screen(' ');

    temporizzatore_aspetta_millisecondi(300);

    prism_screen();

    temporizzatore_aspetta_millisecondi(700);

    clear_screen(' ');

    temporizzatore_aspetta_millisecondi(300);

    prism_screen();

    temporizzatore_aspetta_millisecondi(700);

    clear_screen('m');
    temporizzatore_aspetta_millisecondi(30);
    clear_screen(' ');

    temporizzatore_aspetta_millisecondi(300);

    stampa_regioni_utilizzabili();

    while(1) {
        __asm__ volatile ("hlt");
    }
%>
