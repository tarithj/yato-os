// paging.h
#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

// Constants for page size
#define PAGE_SIZE 4096  // 4 KB page size for x86 architecture

// Entry in the Page Table
typedef struct {
    uint32_t present : 1;   // Page present in memory
    uint32_t rw : 1;        // Read/Write permissions
    uint32_t user : 1;      // User-mode permissions
    uint32_t accessed : 1;  // Page accessed flag
    uint32_t dirty : 1;     // Page dirty flag (for writable pages)
    uint32_t unused : 7;    // Unused bits
    uint32_t frame : 20;    // Frame address (physical address of the page)
} page_table_entry_t; // 32 bits

// Page Table
typedef struct {
    page_table_entry_t entries[1024];  // Each entry corresponds to a 4 KB page
} page_table_t;

// Page Directory
typedef struct {
    page_table_t* tables[1024];  // Each entry points to a page table
} page_directory_t;

// Function to enable paging
void enable_paging(uint32_t page_directory_address);

// Function to allocate a page and return its virtual address
uint32_t allocate_page(page_directory_t* page_directory, uint32_t virtual_address, uint32_t flags);

#endif // PAGING_H
