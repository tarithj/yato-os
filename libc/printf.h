#ifndef PRINTF_H
#define PRINTF_H

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

void kprintf(const char* format, ...);

// Function to convert an integer to a string with various formatting options
char* __int_str(intmax_t i, char b[], int base, bool plusSignIfNeeded, bool spaceSignIfNeeded,
                int paddingNo, bool justify, bool zeroPad);

// Function to display a single character and update the character count
void displayCharacter(char c, int* a);

// Function to display a string and update the character count
void displayString(char* c, int* a);

// Custom implementation of vprintf
int vprintf(const char* format, va_list list);

#endif // VPRINTF_CUSTOM_H
