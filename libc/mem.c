  
#include "mem.h"
#define KERNEL_VIRTUAL_BASE 0xC0000000 // Example base for virtual addresses
#define INITIAL_PHYSICAL_ADDR 0x10000
#define PAGE_SIZE 0x400000  // 4 MB
void memory_copy(uint32_t *source, uint32_t *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

// Function to align a memory address to the nearest 4MB boundary
uint32_t align_to_4MB(uint32_t address) {
    uintptr_t addr = (uintptr_t)address;  // Convert pointer to uintptr_t for bitwise operations
    addr &= ~(PAGE_SIZE - 1);  // Clear the lower 22 bits to align to 4MB boundary
    return addr;  // Convert back to pointer and return
}


void memory_set(uint8_t *dest, uint8_t val, uint32_t len) {
    uint8_t *temp = (uint8_t *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

int memory_compare(const void *ptr1, const void *ptr2, size_t num) {
    const unsigned char *p1 = ptr1;
    const unsigned char *p2 = ptr2;
    
    for (size_t i = 0; i < num; ++i) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    
    return 0;
}
/* This should be computed at link time, but a hardcoded
 * value is fine for now. Remember that our kernel starts
 * at 0x1000 as defined on the Makefile */
static uint32_t free_mem_addr = KERNEL_VIRTUAL_BASE;

/* Implementation is just a pointer to some free memory which
 * keeps growing */
// Function to allocate memory
uint32_t kmalloc(size_t size, int align, uint32_t *phys_addr) {
    // Align address if needed
    if (align && (free_mem_addr & (PAGE_SIZE - 1))) {
        free_mem_addr = (free_mem_addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    }

    // Save physical address if provided
    if (phys_addr) {
        *phys_addr = free_mem_addr - KERNEL_VIRTUAL_BASE + INITIAL_PHYSICAL_ADDR;
    }

    // Get the allocated address and update the free memory address
    uint32_t ret = free_mem_addr;
    free_mem_addr += size; // Move the free memory pointer forward

    return ret;
}