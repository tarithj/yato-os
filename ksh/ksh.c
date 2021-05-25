#include "../drivers/screen.h"
#include "../libc/string.h"
#include "./ksh.h"
#include "../drivers/cmos_rtc.h"

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
    else
    {
        kprint("You said: ");
        kprint(input);
        kprint("\n");
    }
}