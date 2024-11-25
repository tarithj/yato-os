unsigned int get_current_privilege_level() {
    unsigned int current_privilege_level;

    // Inline assembly to read the CS register
    __asm__ __volatile__("movl %%cs, %0" : "=r" (current_privilege_level));

    // Extract the two least significant bits
    current_privilege_level &= 0x03;

    return current_privilege_level;
}