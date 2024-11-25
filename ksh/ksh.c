#include "../drivers/screen.h"
#include "../libc/string.h"
#include "./ksh.h"
#include "../drivers/cmos_rtc.h"
#include "../libc/mem.h"
#include "../cpu/timer.h"
#include "../drivers/sound.h"
#include <stdint.h>
#define LOL " ⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣤⣤⣤⠀⠀⠀⠀⠀⠀⣠⣤⣤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⠀⣿⠀⠀⣤⣶⣷⣶⣿⠁⢹⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⠀⢸⣆⣼⡏⢠⣦⡈⢿⠀⢸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⠀⠘⠛⢻⣧⡈⠛⣀⣿⡀⣸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⠏⢿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣾⣿⡏⠀⠈⣷⠀⠀⠀⠀⠀⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⣀⣀⠀⠀⠀⠀⠀⣀⡤⠶⣿⣿⠿⠤⣤⣴⣿⠀⠀⠀⢀⡀⠀⠀⠄⠀⠀⠀⠀⠀⠀⣀⣴⣶⣿⣷⣶⣦⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠸⣿⠙⠻⣿⣿⣷⣶⣿⠥⡄⠀⠀⠀⠀⠀⠀⠈⠙⢷⡶⠆⠘⠟⠀⠀⠀⠀⠀⠀⠀⢠⣾⣿⠟⠉⠀⠈⠙⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠹⣧⠘⠟⠙⣿⠁⠀⠀⠐⢀⡀⢤⡀⢤⡀⢢⡘⢦⡄⠀⠀⠀⢰⡂⠀⠀⠀⠀⠀⢸⣿⡏⠀⠀⠀⠀⠀⠈⠻⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⢽⣷⠆⢀⡿⠀⠀⠀⣶⠀⠹⡌⣷⠀⣿⣄⣿⡈⣿⡶⢳⡤⢸⣧⠀⠀⠀⠀⠀⢸⣿⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⢰⣶⡄⠿⠠⣇⢸⡄⣿⡀⠀⢻⣿⡶⢿⣌⣻⣇⣿⣿⢈⡇⢸⣏⠳⣄⠀⠀⠀⠀⢿⣿⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⣀⡀⠋⠁⢀⣼⢿⡎⣷⣘⣧⠀⠀⠙⠸⣾⡟⠛⣿⠃⢸⢿⣇⠘⣿⠳⣬⣟⣶⣤⠀⠈⠻⣿⣿⣦⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠉⠳⣾⣀⣼⠿⣌⣷⣼⣧⣽⡆⠀⠀⠀⠈⠁⠀⢉⣀⡜⣸⡏⠀⣿⡆⠀⠈⠁⠀⠀⠀⠀⠈⠙⢿⣿⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⣾⠋⣽⡄⣿⠛⡃⡀⠙⠃⠀⣠⣶⣦⠀⢠⡿⠏⢀⣿⣀⠀⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⣴⣾⣄⣿⣧⡹⣆⠀⠸⣧⣀⣀⣈⣛⣃⡴⠟⠁⣠⣿⣿⡏⣰⣿⡀⢠⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⣠⣾⠟⢁⠀⠀⠉⢷⡙⣦⡀⠀⠈⠉⠉⠉⠉⠶⠤⠞⢻⣿⣿⣷⣿⣿⡇⠀⠀⣀⣀⣀⠀⠀⠀⠀⣠⣾⣿⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠈⣇⠀⡏⣰⢂⣀⠠⡟⠺⢇⣰⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⠘⢿⣿⣿⣿⣿⣿⣶⣾⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠈⠓⠛⠈⠛⠉⠀⠘⠆⢸⣿⡇⠀⠀⠀⠀⠀⠀⠲⣿⣿⣿⣿⣿⣿⣿⠀⢸⣿⡿⣿⣿⣿⣿⣿⣷⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⢸⠀⠀⠀⢀⣠⣾⣿⣿⣄⠀⠀⠀⠀⠀⠀⠘⢻⣿⣿⣿⣿⠏⠀⠀⠛⠀⠙⠻⣿⣿⣿⣿⣿⣿⣿⣶⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠙⣿⣿⣿⣿⣿⣿⡿⣿⣦⠀⠀⠀⠀⠀⠀⣸⣿⣿⣿⡇⠀⡰⠃⠀⠀⠈⠀⠙⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠸⣿⣿⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠛⢻⣿⣿⣿⡇⠈⠀⠀⠀⠀⠀⠀⠀⢻⣿⣿⣿⣿⣿⣿⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠙⠟⠉⠀⠀⠀⠀⠀⠀⠀⠀⣰⡋⠀⢸⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⡀⠀⣼⣿⣿⣿⣿⣿⣿⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⠁⠀⠸⠟⠻⠿⠇⠀⠀⠀⠀⢀⡴⠃⢀⣿⣿⣿⣿⣿⡿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣸⡛⠣⣀⠀⠀⠀⠀⠀⠀⠀⠀⠠⠋⠀⣠⣾⣿⣿⣿⣿⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠿⣷⣦⣭⣀⣀⣀⡀⣸⣃⣀⣠⣴⣶⣶⣿⣿⣿⣿⣯⣀⣀⠀⠀⣠⣾⣷⣤⡀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡤⠌⠉⠉⠁⠀⠀⢹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣶⣄⠀\n⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⡫⠀⠴⠚⡠⠤⠤⠤⣾⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⢿⣿⡿⠿⠿⠿⠿⢿⣿⠿⠿⠿⡿⠟⠛⠀\n⠀⠀⣰⡾⠿⡟⠻⣷⣿⢻⣷⣀⣀⣀⣀⢀⣉⡀⣀⣉⠀⠀⢀⣀⣀⣉⣀⣀⡀⣀⣀⣀⡀⠀⢀⣼⡟⢻⡷⠀⠀⢀⣴⠿⢿⡿⣿⠟⠿⣦⡀\n⠀⠀⣿⣅⠘⠿⣷⣿⡉⠀⣙⣿⣉⣉⠻⣟⠛⣿⡟⢙⣿⠀⣿⠋⢉⠙⢋⠙⢻⣿⣉⡉⠻⣿⠟⠋⠁⢸⡇⠀⣴⡿⢃⣴⡿⣿⡿⠿⢀⣿⠇\n⠀⠀⣸⡿⢷⡦⠈⣿⡇⠈⢿⡟⠉⣥⠀⢹⣆⠘⠀⣾⠇⠀⣿⠀⢸⠂⢸⡇⢸⡟⢉⣅⠀⣇⠀⢿⠇⢸⡇⠀⢻⣧⡈⢿⣶⣿⣷⡶⠈⣿⠀\n⠀⠀⠻⣷⣤⣤⣾⠟⢿⣦⣴⣷⣦⣤⣤⣾⡿⠀⣼⡏⠀⠀⢿⣦⣾⣦⣾⣧⣼⣷⣤⣤⣤⣿⣷⣤⣤⣼⡏⠀⠀⠙⢷⣤⣽⣷⣤⣤⣾⠟⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⢷⣶⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⣉⠀⠀⠀⠀⠀⠀\n\n\0"


// Function to write a pixel to VGA memory buffer
void write_pixel(int x, int y, uint8_t color) {
    uint16_t* vga_buffer = (uint16_t*)VIDEO_ADDRESS_COLOR;
    int offset = y * 320 + x;
    vga_buffer[offset] = color;
}


/**
 * K&R implementation
 */
void uint32_t_to_ascii(uint32_t n, char str[])
{
    int i, sign;
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do
    {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0)
        str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void ksh_handle(char *input)
{
    if (strcmp(input, "DT") == 0)
    {
        struct cmos_rtc_val a = read_rtc();

        char s[2];
        int_to_ascii(a.second, s);
        kprint(s);
        kprint(":");

        char m[2];
        int_to_ascii(a.minute, m);
        kprint(m);
        kprint(":");

        char h[2];
        int_to_ascii(a.hour, h);
        kprint(h);
        kprint(" ");

        char d[2];
        int_to_ascii(a.day, d);
        kprint(d);
        kprint("/");

        char M[2];
        int_to_ascii(a.month, M);
        kprint(M);
        kprint("/");

        char y[10];
        int_to_ascii(a.year, y);
        kprint(y);

        kprint("\n");
    }
    else if (strcmp(input, "PAGE") == 0)
    {
        uint32_t phys_addr;
        uint32_t page = kmalloc(1000, 1, &phys_addr);
        char page_str[16] = "";
        hex_to_ascii(page, page_str);
        char phys_str[16] = "";
        hex_to_ascii(phys_addr, phys_str);
        kprint("Page: ");
        kprint(page_str);
        kprint(", physical address: ");
        kprint(phys_str);
        kprint("\n");
    }
    else if (strcmp(input, "TIMER") == 0)
    {
        kprint("Tick: ");

        uint32_t tick;
        get_tick(&tick);

        char tick_ascii[256];
        int_to_ascii(tick, tick_ascii);
        kprint(tick_ascii);

        kprint("\n");
    }
    else if (strcmp(input, "CLEAR") == 0)
    {
        clear_screen();
    }
    else if (strcmp(input, "FILL") == 0)
    {
        int i = 0x00FFFFFF;
        for (int x = 0; x < MAX_COLS; x++)
        {
            for (int y = 0; y < MAX_ROWS; y++)
            {
                i--;
                kput_pixel(x, y, i); //0x00FFFFFF
            }
        }
    }
    else if (strcmp(input, "FEMBOY") == 0) {
        write_pixel(20, 20, 0xF);
    }
    else if (strcmp(input, "BEEP") == 0)
    {
        beep();
    }
    else if (strcmp(input, "HELP") == 0)
    {
        kprint("\n");
        kprint("DT           displays date time info in UTC using cmos\n");
        kprint("PAGE         allocates a page\n");
        kprint("TIMER        get current tick\n");
        kprint("CLEAR        clear screen\n");
        kprint("FILL         fills the screen\n");
        kprint("BEEP         beeps using the pc speaker\n");
    }
    else
    {
        kprint("You said: ");
        kprint(input);
        kprint("\n");
    }
    kprint("> ");
}