// file: src/kernel.h
#ifndef KERNEL_H
#define KERNEL_H

#define SCREEN_WIDTH_IN_CHARACTERS 80
#define SCREEN_HEIGHT_IN_CHARACTERS 25
#define VIDEO_MEMORY_ADDRESS 0xB8000
#define DEFAULT_COLOR_ATTRIBUTE 0x0F

void kernel_main();
void clear_screen();
void stampa_carattere(char carattere);
void print(char *str);

#endif