#include "memory.h"
#include "nofreemem.h"
#include "type.h"
#include "isr.h"
#include "screen.h"
#include "mem.h"
#include "frame.h"

u32  vaddr_to_paddr(u32 address) {
    u32 pageDirIdx = ((address>>22) & 0x3FF);
    page_table_t *pageTable = current_directory->tables[pageDirIdx];
    if (pageTable == 0) {
        kprint(" 1.1 page table is nil\n");
    }
    u32 pageIdx = (address>>12) & 0x3FF;
    page_t *page = &(pageTable->pages[pageIdx]);
    if (page == 0) {
        kprint(" 1.2 page is nil\n");
    }
    u32 phyAddr = (u32)(page->frame * 0x1000) + (u32)(address&0x00000FFF);
    return phyAddr;
}


void switch_page_directory(page_directory_t *dir)
{
    asm("mov %0, %%cr3" ::"r"(&dir->tablesPhysical));
    u32 cr0;
    asm("mov %%cr0, %0"
        : "=r"(cr0));
    cr0 |= 0x80000000;
    asm("mov %0, %%cr0" ::"r"(cr0));
}

/*
   virtual address <--> page
*/
page_t *get_page(u32 address, int make, page_directory_t *dir)
{
    address /= 0x1000;
    u32 table_idx = address / 1024;
    if (dir->tables[table_idx])
    {
        return &dir->tables[table_idx]->pages[address % 1024];
    }
    else if (make)
    {
        u32 tmp = 0;
        dir->tables[table_idx] = (page_table_t *)kmalloc_ap_nofree(sizeof(page_table_t), 1, &tmp);
        if (dir->tables[table_idx] == 0 && kheap_ready()) {
            dir->tables[table_idx] = (page_table_t *)kmalloc_a(sizeof(page_table_t), 1);
            tmp = vaddr_to_paddr(dir->tables[table_idx]);
        }
        if (dir->tables[table_idx] == 0)
            return 0;
        kmemset((u8 *)dir->tables[table_idx], 0, sizeof(page_table_t));
        kprint("page alloc mem, vaddr: ");
        kprint_hex(dir->tables[table_idx]);
        kprint(" phy: ");
        kprint_hex(tmp);
        kprint("\n");
        dir->tablesPhysical[table_idx] = tmp | 0x7;
        return &dir->tables[table_idx]->pages[address % 1024];
    }
    else
    {
        return 0;
    }
}
