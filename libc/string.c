#include "string.h"
#include <stdint.h>
/**
 * K&R implementation
 */
void int_to_ascii(int n, char str[])
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

/* K&R */
void reverse(char s[])
{
    int c, i, j;
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--)
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* K&R */
int strlen(char s[])
{
    int i = 0;
    while (s[i] != '\0')
        ++i;
    return i;
}

void append(char s[], char n)
{
    int len = strlen(s);
    s[len] = n;
    s[len + 1] = '\0';
}

void backspace(char s[])
{
    int len = strlen(s);
    s[len - 1] = '\0';
}

/* K&R 
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
int strcmp(char s1[], char s2[])
{
    int i;
    for (i = 0; s1[i] == s2[i]; i++)
    {
        if (s1[i] == '\0')
            return 0;
    }
    return s1[i] - s2[i];
}

void hex_to_ascii(int n, char str[]) {
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    int32_t tmp;
    int i;
    for (i = 28; i > 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) continue;
        zeros = 1;
        if (tmp > 0xA) append(str, tmp - 0xA + 'a');
        else append(str, tmp + '0');
    }

    tmp = n & 0xF;
    if (tmp >= 0xA) append(str, tmp - 0xA + 'a');
    else append(str, tmp + '0');
}

char* strcpy(char* dest, const char* src) {
    char* original_dest = dest;  // Store the starting address of dest
    
    // Copy characters from src to dest until we hit the null terminator
    while ((*dest++ = *src++) != '\0') {
        // Nothing inside the loop, the assignment does all the work
    }
    
    return original_dest;  // Return the original destination pointer
}


char* strcat(char* dest, const char* src) {
    char* original_dest = dest;  // Store the starting address of dest
    
    // Move dest pointer to the null-terminator of the existing string
    while (*dest != '\0') {
        dest++;  // Move the pointer until the end of dest string
    }
    
    // Append each character from src to the end of dest
    while ((*dest++ = *src++) != '\0') {
        // Nothing inside the loop, the assignment does all the work
    }
    
    return original_dest;  // Return the original destination pointer
}
