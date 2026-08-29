// file: src/kernel.h
#ifndef KERNEL_H
#define KERNEL_H

#define SCREEN_WIDTH_IN_CHARACTERS 80
#define SCREEN_HEIGHT_IN_CHARACTERS 25
#define VIDEO_MEMORY_ADDRESS 0xB8000
#define DEFAULT_COLOR_ATTRIBUTE 0x0F

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define VGA_BLUE        0x01
#define VGA_GREEN       0x02
#define VGA_CYAN        0x03
#define VGA_RED         0x04
#define VGA_MAGENTA     0x05
#define VGA_BROWN       0x06
#define VGA_LIGHT_GRAY  0x07
#define VGA_DARK_GRAY   0x08
#define VGA_LIGHT_BLUE  0x09
#define VGA_LIGHT_GREEN 0x0A
#define VGA_LIGHT_CYAN  0x0B
#define VGA_LIGHT_RED   0x0C
#define VGA_LIGHT_MAGENTA 0x0D
#define VGA_YELLOW      0x0E
#define VGA_WHITE       0x0F

void kernel_main();
void clear_screen();
void stampa_carattere(char carattere);
void print(char *str);

#endif