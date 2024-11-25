#include "../libc/string.h" 
#include "../libc/mem.h"
#include "./paging.h"
#include "../libc/function.h"
#define PAGE_SIZE 4096  // 4 KB page size for x86 architecture


uint32_t is_paging_enabled(void) {
    uint32_t cr0;
    // Read the value of CR0 register into 'cr0'
    __asm__("mov %%cr0, %0" : "=r"(cr0));
    
    // Check if the Paging (PG) bit is set
    return (cr0 & 0x80000000) != 0;
}

// Function to enable paging
void enable_paging(uint32_t page_directory_address) {
    // Load the page directory base address into CR3 register
    __asm__("mov %0, %%cr3" : : "r"(page_directory_address));

    // Set the paging bit in the CR0 register
    uint32_t cr0;
    __asm__("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;  // Set the paging bit
    __asm__("mov %0, %%cr0" : : "r"(cr0));
}


// Function to allocate a frame (physical page)
uint32_t allocate_frame(void) {
    // Implement your frame allocation logic here
    // This could involve managing a free list, marking frames as used, etc.
    // For simplicity, let's assume a simple incrementing frame number
    static uint32_t frame_counter = 0;
    return frame_counter++;
}

// Function to allocate a page and return its virtual address
uint32_t allocate_page(page_directory_t* page_directory, uint32_t virtual_address, uint32_t flags) {
    // Assume that the page directory and page tables are already set up
    UNUSED(flags);
    // Calculate the indices for the page directory and page table
    uint32_t directory_index = (virtual_address >> 22) & 0x3FF;
    uint32_t table_index = (virtual_address >> 12) & 0x3FF;

    // Check if the page table for this directory entry exists; if not, create one
    if (!page_directory->tables[directory_index]) {
        // Allocate a page for the new page table
        page_directory->tables[directory_index] = (page_table_t*)kmalloc(sizeof(page_table_t), 1, 0);

        // Ensure that the page table is cleared (set all entries to 0)
        memory_set((uint32_t*) (page_directory->tables[directory_index]), 0, sizeof(page_table_t));

        // Set the present bit and read/write permissions for the directory entry
        page_directory->tables[directory_index]->entries[directory_index].present = 1;
        page_directory->tables[directory_index]->entries[directory_index].rw = 1;
    }

    // Allocate a frame (physical page) and set the corresponding entry in the page table
    uint32_t frame = allocate_frame();
    page_directory->tables[directory_index]->entries[table_index].frame = frame;
    page_directory->tables[directory_index]->entries[table_index].present = 1;
    page_directory->tables[directory_index]->entries[table_index].rw = 1;  // Read/Write permissions

    // Calculate the virtual address of the allocated page
    uint32_t page_address = (directory_index << 22) | (table_index << 12);
    
    // Optionally, flush TLB (Translation Lookaside Buffer) for the new mapping
    // This ensures that the CPU uses the updated page tables immediately
    __asm__("invlpg (%0)" : : "a"(page_address));

    return page_address;
}
