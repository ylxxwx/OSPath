#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "../cpu/isr.h"

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

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

void init_memory();
void page_fault(registers_t regs);

#endif // _MEMORY_H_