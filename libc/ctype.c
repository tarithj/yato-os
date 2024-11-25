#include <stdbool.h>
/**
 * Check if a character is a decimal digit (0-9).
 * 
 * @param c The character to check.
 * @return true if c is a digit, false otherwise.
 */
bool isdigit(char c) {
    return (c >= '0' && c <= '9');
}