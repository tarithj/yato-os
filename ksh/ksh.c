#include "../drivers/screen.h"
#include "../libc/string.h"
#include "./ksh.h"
#include "../drivers/cmos_rtc.h"
#include "../libc/mem.h"
#include <stdint.h>

void ksh_handle(char *input)
{
    kprint("> ");
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
    else
    {
        kprint("You said: ");
        kprint(input);
        kprint("\n");
    }
}