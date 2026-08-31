ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0x0100000;
    inizio_kernel = .;   /* indirizzo fisico di partenza del kernel */

    .text : ALIGN(4096)
    {
        *(.text)
    }

    .rodata : ALIGN(4096)
    {
        *(.rodata)
    }

    .data : ALIGN(4096)
    {
        *(.data)
    }

    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss)
    }

    fine_kernel = .;   /* indirizzo fisico subito dopo l'ultimo byte del kernel */
}
