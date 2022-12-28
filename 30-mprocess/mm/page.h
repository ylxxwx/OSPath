#ifndef _PAGE_H_
#define _PAGE_H_

#include "isr.h"

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))
#define PAGE_SIZE (4*1024)

typedef struct page {
   u32 present :1;
   u32 rw      :1;
   u32 user    :1;
   u32 accessed:1;
   u32 dirty   :1;
   u32 unused  :7;
   u32 frame   :20;
}page_t;

typedef struct page_table {
    page_t pages[1024];
}page_table_t;

typedef struct page_directory {
   page_table_t *tables[1024];
   u32 tablesPhysical[1024];
   //u32 physicalAddr;
} page_directory_t;

extern page_directory_t * current_directory;
extern page_directory_t * kernel_directory;

page_t *get_page(u32 address, int make, page_directory_t *dir);
void switch_page_directory(page_directory_t *dir);
void page_fault(registers_t *regs);
u32  vaddr_to_paddr(u32 address);

#endif //_PAGE_H_
