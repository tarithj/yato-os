#include "../libc/string.h"
#include <stdbool.h> 
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include "../libc/mem.h"
#include "../libc/ctype.h"
#include "../drivers/screen.h"

char* __int_str(intmax_t i, char b[], int base, bool plusSignIfNeeded, bool spaceSignIfNeeded,
                int paddingNo, bool justify, bool zeroPad) {
    
    char digit[32] = {0};
    memory_set((uint8_t *)digit, 0, 32);
    strcpy(digit, "0123456789");
    
    if (base == 16) {
        strcat(digit, "ABCDEF");
    } else if (base == 17) {
        strcat(digit, "abcdef");
        base = 16;
    }
    
    char* p = b;
    if (i < 0) {
        *p++ = '-';
        i *= -1;
    } else if (plusSignIfNeeded) {
        *p++ = '+';
    } else if (!plusSignIfNeeded && spaceSignIfNeeded) {
        *p++ = ' ';
    }
    
    intmax_t shifter = i;
    do {
        ++p;
        shifter = shifter / base;
    } while (shifter);
    
    *p = '\0';
    do {
        *--p = digit[i % base];
        i = i / base;
    } while (i);
    
    int padding = paddingNo - (int)strlen(b);
    if (padding < 0) padding = 0;
    
    if (justify) {
        while (padding--) {
            if (zeroPad) {
                b[strlen(b)] = '0';
            } else {
                b[strlen(b)] = ' ';
            }
        }
        
    } else {
        char a[256] = {0};
        while (padding--) {
            if (zeroPad) {
                a[strlen(a)] = '0';
            } else {
                a[strlen(a)] = ' ';
            }
        }
        strcat(a, b);
        strcpy(b, a);
    }
    
    return b;
}

void displayCharacter(char c, int* a) {
    static char buffer[2]; // Static or stack-allocated buffer for 1 character + null terminator
    buffer[0] = c;         // Place the character in the first position
    buffer[1] = '\0';      // Null-terminate the string
    kprint(buffer);
    *a += 1;
}

void displayString(char* c, int* a) {
    for (int i = 0; c[i]; ++i) {
        displayCharacter(c[i], a);
    }
}



int vprintf (const char* format, va_list list)
{
    int chars        = 0;
    char intStrBuffer[256] = {0};
    
    for (int i = 0; format[i]; ++i) {
        
        char specifier   = '\0';
        char length      = '\0';
        
        int  lengthSpec  = 0; 
        int  precSpec    = 0;
        bool leftJustify = false;
        bool zeroPad     = false;
        bool spaceNoSign = false;
        bool altForm     = false;
        bool plusSign    = false;
        bool emode       = false;
        int  expo        = 0;
        
        if (format[i] == '%') {
            ++i;
            
            bool extBreak = false;
            while (1) {
            
                switch (format[i]) {
                    case '-':
                        leftJustify = true;
                        ++i;
                        break;
                        
                    case '+':
                        plusSign = true;
                        ++i;
                        break;
                        
                    case '#':
                        altForm = true;
                        ++i;
                        break;
                        
                    case ' ':
                        spaceNoSign = true;
                        ++i;
                        break;
                        
                    case '0':
                        zeroPad = true;
                        ++i;
                        break;
                        
                    default:
                        extBreak = true;
                        break;
                }
                
                if (extBreak) break;
            }
            
            while (isdigit(format[i])) {
                lengthSpec *= 10;
                lengthSpec += format[i] - 48;
                ++i;
            }
            
            if (format[i] == '*') {
                lengthSpec = va_arg(list, int);
                ++i;
            }
            
            if (format[i] == '.') {
                ++i;
                while (isdigit(format[i])) {
                    precSpec *= 10;
                    precSpec += format[i] - 48;
                    ++i;
                }
                
                if (format[i] == '*') {
                    precSpec = va_arg(list, int);
                    ++i;
                }
            } else {
                precSpec = 6;
            }
            
            if (format[i] == 'h' || format[i] == 'l' || format[i] == 'j' ||
                   format[i] == 'z' || format[i] == 't' || format[i] == 'L') {
                length = format[i];
                ++i;
                if (format[i] == 'h') {
                    length = 'H';
                } else if (format[i] == 'l') {
                    length = 'q';
                    ++i;
                }
            }
            specifier = format[i];
            
            memory_set((uint8_t *)intStrBuffer, 0, 256);
            
            int base = 10;
            if (specifier == 'o') {
                base = 8;
                specifier = 'u';
                if (altForm) {
                    displayString("0", &chars);
                }
            }
            if (specifier == 'p') {
                base = 16;
                length = 'z';
                specifier = 'u';
            }
            switch (specifier) {
                case 'X':
                    base = 16;
                    __attribute__((fallthrough));
                case 'x':
                    base = base == 10 ? 17 : base;
                    if (altForm) {
                        displayString("0x", &chars);
                    }
                    
                    __attribute__((fallthrough));
                case 'u':
                {
                    switch (length) {
                        case 0:
                        {
                            unsigned int integer = va_arg(list, unsigned int);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 'H':
                        {
                            unsigned char integer = (unsigned char) va_arg(list, unsigned int);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 'h':
                        {
                            unsigned short int integer = va_arg(list, unsigned int);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 'l':
                        {
                            unsigned long integer = va_arg(list, unsigned long);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 'q':
                        {
                            unsigned long long integer = va_arg(list, unsigned long long);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 'j':
                        {
                            uintmax_t integer = va_arg(list, uintmax_t);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 'z':
                        {
                            size_t integer = va_arg(list, size_t);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        case 't':
                        {
                            ptrdiff_t integer = va_arg(list, ptrdiff_t);
                            __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                            displayString(intStrBuffer, &chars);
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }
                    
                case 'd':
                case 'i':
                {
                    switch (length) {
                    case 0:
                    {
                        int integer = va_arg(list, int);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 'H':
                    {
                        signed char integer = (signed char) va_arg(list, int);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 'h':
                    {
                        short int integer = va_arg(list, int);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 'l':
                    {
                        long integer = va_arg(list, long);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 'q':
                    {
                        long long integer = va_arg(list, long long);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 'j':
                    {
                        intmax_t integer = va_arg(list, intmax_t);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 'z':
                    {
                        size_t integer = va_arg(list, size_t);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    case 't':
                    {
                        ptrdiff_t integer = va_arg(list, ptrdiff_t);
                        __int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
                        displayString(intStrBuffer, &chars);
                        break;
                    }
                    default:
                        break;
                    }
                    break;
                }
                    
                case 'c':
                {
                    if (length == 'l') {
                        displayCharacter(va_arg(list, int), &chars);
                    } else {
                        displayCharacter(va_arg(list, int), &chars);
                    }
                    
                    break;
                }
                    
                case 's':
                {
                    displayString(va_arg(list, char*), &chars);
                    break;
                }
                    
                case 'n':
                {
                    switch (length) {
                        case 'H':
                            *(va_arg(list, signed char*)) = chars;
                            break;
                        case 'h':
                            *(va_arg(list, short int*)) = chars;
                            break;
                            
                        case 0: {
                            int* a = va_arg(list, int*);
                            *a = chars;
                            break;
                        }
                            
                        case 'l':
                            *(va_arg(list, long*)) = chars;
                            break;
                        case 'q':
                            *(va_arg(list, long long*)) = chars;
                            break;
                        case 'j':
                            *(va_arg(list, intmax_t*)) = chars;
                            break;
                        case 'z':
                            *(va_arg(list, size_t*)) = chars;
                            break;
                        case 't':
                            *(va_arg(list, ptrdiff_t*)) = chars;
                            break;
                        default:
                            break;
                    }
                    break;
                }
                   
                case 'e':
                case 'E':
                    emode = true;
                    __attribute__((fallthrough));
                    
                case 'f':
                case 'F':
                case 'g':
                case 'G':
                {
                    double floating = va_arg(list, double);
                    
                    while (emode && floating >= 10) {
                        floating /= 10;
                        ++expo;
                    }
                    
                    int form = lengthSpec - precSpec - expo - (precSpec || altForm ? 1 : 0);
                    if (emode) {
                        form -= 4;      // 'e+00'
                    }
                    if (form < 0) {
                        form = 0;
                    }
                    
                    __int_str(floating, intStrBuffer, base, plusSign, spaceNoSign, form, \
                              leftJustify, zeroPad);
                    
                    displayString(intStrBuffer, &chars);
                    
                    floating -= (int) floating;
                    
                    for (int i = 0; i < precSpec; ++i) {
                        floating *= 10;
                    }
                    intmax_t decPlaces = (intmax_t) (floating + 0.5);
                    
                    if (precSpec) {
                        displayCharacter('.', &chars);
                        __int_str(decPlaces, intStrBuffer, 10, false, false, 0, false, false);
                        intStrBuffer[precSpec] = 0;
                        displayString(intStrBuffer, &chars);
                    } else if (altForm) {
                        displayCharacter('.', &chars);
                    }
                    
                    break;
                }
                    
                    
                case 'a':
                case 'A':
                    //ACK! Hexadecimal floating points...
                    break;
                
                default:
                    break;
            }
            
            if (specifier == 'e') {
                displayString("e+", &chars);
            } else if (specifier == 'E') {
                displayString("E+", &chars);
            }
            
            if (specifier == 'e' || specifier == 'E') {
                __int_str(expo, intStrBuffer, 10, false, false, 2, false, true);
                displayString(intStrBuffer, &chars);
            }
            
        } else {
            displayCharacter(format[i], &chars);
        }
    }
    
    return chars;
}

__attribute__ ((format (printf, 1, 2))) int printf (const char* format, ...)
{
    va_list list;
    va_start (list, format);
    int i = vprintf (format, list);
    va_end (list);
    return i;
    
}
void kprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);          // Initialize va_list with the last named parameter
    vprintf(format, args);           // Pass va_list to vprintf
    va_end(args);                    // Clean up
}