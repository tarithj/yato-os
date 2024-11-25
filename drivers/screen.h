#ifndef SCREEN_H
#define SCREEN_H

#include "../cpu/type.h"
#include <stdint.h>

#include "../drivers/multiboot.h"

// Define a structure to hold framebuffer info
typedef struct {
    uint32_t fb_addr;   // Framebuffer address
    uint32_t fb_pitch;  // Number of bytes per row
    uint32_t fb_width;  // Width in pixels
    uint32_t fb_height; // Height in pixels
    uint32_t fb_bpp;    // Bits per pixel
} framebuffer_info_t;


#define VIDEO_ADDRESS_MONOCHROME 0xb0000
#define VIDEO_ADDRESS_COLOR 0xb8000
#define MAX_ROWS 600
#define MAX_COLS 800
#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4

/* Screen i/o ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5
void vga_set_mode(unsigned char mode);

/* Public kernel API */
void clear_screen();
void kprint_at(char *message, int col, int row);
void kprint(char *message);
void kprint_backspace();
void kput_pixel(int x, int y, int color);
void kprintln(char *message);
void draw_char(int x, int y, char character, int color);
int get_cursor_offset();
int get_offset_col(int offset);
int get_offset_row(int offset);
void setVideoMode(unsigned char mode);
void init_framebuffer(vbe_mode_info_structure_t *mb_info);
#endif