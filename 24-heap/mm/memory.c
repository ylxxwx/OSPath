#include "memory.h"
#include "type.h"
#include "isr.h"
#include "screen.h"
#include "mem.h"
#include "frame.h"

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
        u32 tmp;
        dir->tables[table_idx] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), 1, &tmp);
        kmemset((u8 *)dir->tables[table_idx], 0, 4096);
        dir->tablesPhysical[table_idx] = tmp | 0x7;
        return &dir->tables[table_idx]->pages[address % 1024];
    }
    else
    {
        return 0;
    }
}

extern u32 *frames;
extern u32 nframes;
extern u32 free_mem_addr;
page_directory_t * current_directory = 0;

void init_memory()
{
    u32 mem_end_page = 0x1000000;
    nframes = mem_end_page / 0x1000;

    u32 addr;
    page_directory_t *k_dir = (page_directory_t *)kmalloc_ap(sizeof(page_directory_t), 1, &addr);
    kmemset((u8 *)k_dir, 0, sizeof(page_directory_t));

    current_directory = k_dir;

    frames = (u32 *)kmalloc_a(INDEX_FROM_BIT(nframes));
    kmemset((u8 *)frames, 0, INDEX_FROM_BIT(nframes));

    int i = 0;
    u32 top = free_mem_addr;
    if (free_mem_addr < 0x200000)
        top = 0x200000;

    while (i < top)
    {
        page_t *pPage = get_page(i, 1, k_dir);
        if (pPage == 0)
        {
            kprint("get paget return 0\n");
            return;
        }
        alloc_frame(pPage, 0, 0);
        
        i += 0x1000;
    }

    register_interrupt_handler(14, page_fault);

    switch_page_directory(k_dir);
}
