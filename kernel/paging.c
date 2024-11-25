#include "paging.h"
#include "../libc/printf.h"

uintptr_t phys_to_virt(uintptr_t phys)
{
    return phys + 0xC0000000;
}

uintptr_t virt_to_phys(uintptr_t virt)
{
    return virt - 0xC0000000;
}



#include <stdint.h>
#include <stddef.h> // For NULL

#define PAGE_SIZE 4096
#define MAX_PAGES 1024  // Assume 1024 pages for simplicity

// Bitmap to track free/used pages (1 bit per page)
static uint8_t page_bitmap[MAX_PAGES / 8];

// Physical memory start
uintptr_t physical_memory_start = 4000;  // Set during initialization

// Utility macros
#define BITMAP_INDEX(n) ((n) / 8)
#define BITMAP_OFFSET(n) ((n) % 8)

// Mark a page in the bitmap as used or free
static inline void mark_page(size_t page, int used) {
    if (used) {
        page_bitmap[BITMAP_INDEX(page)] |= (1 << BITMAP_OFFSET(page));
    } else {
        page_bitmap[BITMAP_INDEX(page)] &= ~(1 << BITMAP_OFFSET(page));
    }
}

// Check if a page is free
static inline int is_page_free(size_t page) {
    return !(page_bitmap[BITMAP_INDEX(page)] & (1 << BITMAP_OFFSET(page)));
}

// Allocate a page
void* alloc_page() {
    for (size_t i = 0; i < MAX_PAGES; i++) {
        if (is_page_free(i)) {
            mark_page(i, 1);  // Mark page as used
            return (void*)(physical_memory_start + i * PAGE_SIZE);
        }
    }
    return NULL;  // Out of memory
}

// Free a page
void free_page(void* addr) {
    uintptr_t page = ((uintptr_t)addr - physical_memory_start) / PAGE_SIZE;
    if (page < MAX_PAGES) {
        mark_page(page, 0);  // Mark page as free
    }
}
#include "../cpu/isr.h"

void page_fault_handler(registers_t *r)
{
     uintptr_t fault_addr;
     asm volatile("mov %%cr2, %0" : "=r" (fault_addr));

     int present = !(r->err_code & 0x1);
     int rw = r->err_code & 0x2;
     int us = r->err_code & 0x4;
     int reserved = r->err_code & 0x8;
     /* int id = regs->err_code & 0x10; */

     kprintf("Page fault! ( ");
     if (present) { kprintf("present "); }
  if (rw) { kprintf("read-only "); }
     if (us) { kprintf("user-mode "); }
     if (reserved) { kprintf("reserved "); }
     kprintf(") at 0x%X\n", fault_addr);
     while (1)
     {
        /* code */
     };
 }
#define KERNEL_VIRTUAL_BASE 0xC0000000 // Example base for virtual addresses


void paging_install(void)
{
    /* find first 4KB aligned address after the end of the kernel */
    /* create page directory for 4GB of RAM at this address */
    uintptr_t page_directory = (uintptr_t)alloc_page();

    //KASSERT(page_directory);
    //kprintf("page directory: 0x%x\n", virt_to_phys(page_directory));

    unsigned int pde = 0;
    /* set bits for all page directory entries */
    for (pde = 0; pde < 1024; pde++) {
        /* ((uintptr_t*)page_directory)[pde] = 0 | 2; /1* supervisor level, read/write, not present *1/ */
        ((uintptr_t*)page_directory)[pde] = 0 | 6;  /* usermode level, read/write, not present */
    }

    /* map first 16MB in 4 page tables */
    uintptr_t address = 0x0;
    unsigned int pidx = 0;
    for (pidx = KERNEL_VIRTUAL_BASE >> 22; pidx < (KERNEL_VIRTUAL_BASE >> 22) + 4; pidx++) {
        uintptr_t page_table = (uintptr_t)alloc_page();
        //KASSERT(page_table);

        unsigned int pte;
        for (pte = 0; pte < 1024; pte++) {
            /* ((uint32_t*)page_table)[pte] = address | 3;  /1* supervisor level, read/write, present *1/ */
            ((uint32_t*)page_table)[pte] = address | 7; /* usermode level, read/write, present */
            address += 0x1000;  /* next page address */
        }

        /* set PHYSICAL addresses of page table(s) in page directory */
        /* ((uintptr_t*)page_directory)[pidx] = virt_to_phys(page_table) | 3; /1* supervisor level, read/write, present *1/ */
        ((uintptr_t*)page_directory)[pidx] = virt_to_phys(page_table) | 7; /* usermode level, read/write, present */
        //kprintf("page table %u: 0x%x\n", pidx, virt_to_phys(page_table));
    }

    register_interrupt_handler(14, page_fault_handler);

    /* move PHYSICAL page directory address into cr3 */
    asm volatile("mov %0, %%cr3":: "r" (virt_to_phys(page_directory)));

    /* clear 4MB page bit since we're switching to 4KB pages */
    uint32_t cr4;
    asm volatile("mov %%cr4, %0": "=r" (cr4));
    cr4 &= ~(0x00000010);
    asm volatile("mov %0, %%cr4":: "r" (cr4));

    /* read cr0, set paging bit, write it back */
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r" (cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "r" (cr0));
}