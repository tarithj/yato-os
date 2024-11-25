void convert_reverse(char *str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void convert_int_to_string(int num, char *str) {
    int i = 0;
    int is_negative = 0;
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    // Handle 0 explicitly, otherwise empty string will be printed
    if (num == 0) {
        str[i++] = '0';
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }

    // If number is negative, add '-' to the string
    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0'; // Add null terminator

    // Reverse the string
    convert_reverse(str, i);

}
