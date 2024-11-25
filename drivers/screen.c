#include "screen.h"
#include "../cpu/port.h"
#include "../libc/mem.h"
#include "bios.h"

#include <stdint.h>
#include "font.h"
#include <stdint.h>

framebuffer_info_t framebuffer_out;

// Assume 'mb_info' is a pointer to the multiboot information structure
void init_framebuffer(vbe_mode_info_structure_t *mb_info) {
    
    framebuffer_out.fb_addr = mb_info->framebuffer;    // Framebuffer address is at offset 40 bytes (10th element, assuming uint32_t)
    framebuffer_out.fb_pitch = mb_info->pitch;   // Framebuffer pitch
    framebuffer_out.fb_width = mb_info->width;   // Framebuffer width
    framebuffer_out.fb_height = mb_info->height;  // Framebuffer height
    framebuffer_out.fb_bpp =32;     // Framebuffer bits per pixel
    
    // Now you have the framebuffer information and can start writing to it
}





/* Declaration of private functions */
void set_cursor_offset(int offset);
uint8_t* get_video_mem();
int print_char(char c, int col, int row, char attr, uint8_t* vidmem);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);
static inline void put_pixel(int x, int y, uint32_t val);

int get_cursor_offset();

/**********************************************************
 * Public Kernel API functions                            *
 **********************************************************/

void draw_char(int x, int y, char character, int color) {
    int xx,yy;
    int set = x+y+character;
    char *bitmap = font8x8_basic[(int)character];
    for (xx=0; xx < 8; xx++) {
        for (yy=0; yy < 8; yy++) {
            set = (bitmap[xx] & 1 << yy) ==0;
            kput_pixel(y+yy,x+xx,set ?0 :color);
        }
    }
}

int rowS = 0;
int colS = 0;
/**
 * Print a message on the specified location
 * If col, row, are negative, we will use the current offset
 */
void kprint_at(char *message, int col, int row)
{
    
    /* Set cursor if col/row are negative */
    if (col >= 0 && row >= 0){
        rowS = row;
        colS = col;
    }

     /* Loop through message and print it */
    int i = 0;
    while (message[i] != 0)
    {
        if(message[i] == '\n') {
            if(rowS+8 > MAX_ROWS) {
                rowS = 0;
                colS = 0;
                continue;
            }
            rowS+=8; colS =0; i++; continue;
        }
        int padding = 2;
        draw_char(rowS, colS, message[i++], 234); //print_char(message[i++], col, row, WHITE_ON_BLACK, get_video_mem());
        /* Compute row/col for next iteration */
        if (rowS+8+padding > MAX_ROWS) {
            rowS = rowS+8+padding;
        }
        if (colS+8+padding > MAX_COLS) {
            rowS = 0;
            colS = 0;
        } else {
            colS = colS+8+padding;
        }
    }
}

void kprint(char *message)
{
    kprint_at(message, -1, -1);
}

void kprintln(char *message)
{
    kprint_at(message, -1, -1);
    kprint_at("\n", -1, -1);
}


void kprint_backspace()
{
    colS -= 8 + 2;
    if (colS <= 0) {
        colS = 0;
    }
    draw_char(rowS, colS, ' ', 234);
}

void kput_pixel(int x, int y, int color)
{
    put_pixel(x, y, color);
}

int get_cursor_offset()
{
    /* Use the VGA ports to get the current cursor position
     * 1. Ask for high byte of the cursor offset (data 14)
     * 2. Ask for low byte (data 15)
     */
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8; /* High byte: << 8 */
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2; /* Position * size of character cell */
}

/**********************************************************
 * Private kernel functions                               *
 **********************************************************/

/**
 * Returns a pointer to the appropriate video memory location according to the video type
*/
uint8_t* get_video_mem() {
    uint8_t* vidmem;
    if (get_bios_area_video_type() == VIDEO_TYPE_COLOR) {
        vidmem = (uint8_t*)VIDEO_ADDRESS_COLOR;
    } else {
        vidmem = (uint8_t*)VIDEO_ADDRESS_MONOCHROME;
    }
    return vidmem;
}

/**
 * Innermost print function for our kernel, directly accesses the video memory 
 *
 * If 'col' and 'row' are negative, we will print at current cursor location
 * If 'attr' is zero it will use 'white on black' as default
 * Returns the offset of the next character
 * Sets the video cursor to the returned offset
 */
int print_char(char c, int col, int row, char attr, uint8_t* vidmem)
{
    if (!attr)
        attr = WHITE_ON_BLACK;

    /* Error control: print a red 'E' if the coords aren't right */
    if (col >= MAX_COLS || row >= MAX_ROWS)
    {
        vidmem[2 * (MAX_COLS) * (MAX_ROWS)-2] = 'E';
        vidmem[2 * (MAX_COLS) * (MAX_ROWS)-1] = RED_ON_WHITE;
        return get_offset(col, row);
    }

    int offset;
    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else
        offset = get_cursor_offset();

    if (c == '\n')
    {
        row = get_offset_row(offset);
        offset = get_offset(0, row + 1);
    }
    else if (c == 0x08)
    { /* Backspace */
        vidmem[offset] = ' ';
        vidmem[offset + 1] = attr;
    }
    else
    {
        vidmem[offset] = c;
        vidmem[offset + 1] = attr;
        offset += 2;
    }

    /* Check if the offset is over screen size and scroll */
    if (offset >= MAX_ROWS * MAX_COLS * 2)
    {
        int i;
        for (i = 1; i < MAX_ROWS; i++)
            memory_copy((uint32_t *)(get_offset(0, i) + VIDEO_ADDRESS_COLOR),
                        (uint32_t *)(get_offset(0, i - 1) + VIDEO_ADDRESS_COLOR),
                        MAX_COLS * 2);

        /* Blank last line */
        char *last_line = (char *)(get_offset(0, MAX_ROWS - 1) + (uint8_t *)VIDEO_ADDRESS_COLOR);
        for (i = 0; i < MAX_COLS * 2; i++)
            last_line[i] = 0;

        offset -= 2 * MAX_COLS;
    }

    set_cursor_offset(offset);
    return offset;
}

void set_cursor_offset(int offset)
{
    /* Similar to get_cursor_offset, but instead of reading we write data */
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (uint8_t)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (uint8_t)(offset & 0xff));
}

void clear_screen()
{    
    for (int x = 0; x < MAX_COLS; x++)
    {
        for (int y = 0; y < MAX_ROWS; y++)
        {
            kput_pixel(x, y, 0); //0x00FFFFFF
        }
    }
    colS = 0;
    rowS = 0;
}

/* void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    uint32_t *pixel = (uint32_t *)(framebuffer_out.fb_addr + y * framebuffer_out.fb_pitch + x * (framebuffer_out.fb_bpp / 8));
    *pixel = color;
}
 */

static inline void put_pixel(int x, int y, uint32_t val) {
    *(uint32_t *) (framebuffer_out.fb_addr + x * 4 + framebuffer_out.fb_pitch * y) = val;
    //uint32_t *buffer = (uint32_t *) framebuffer_out.fb_addr;
    //buffer[framebuffer_out.fb_width * y + x] = val;
}



int get_offset(int col, int row) { return 2 * (row * MAX_COLS + col); }
int get_offset_row(int offset) { return offset / (2 * MAX_COLS); }
int get_offset_col(int offset) { return (offset - (get_offset_row(offset) * 2 * MAX_COLS)) / 2; }

#define VGA_CONTROLLER_ADDR 0x10
extern void vga_set_mode(unsigned char mode);
