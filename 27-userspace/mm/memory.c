#include "memory.h"
#include "nofreemem.h"
#include "type.h"
#include "isr.h"
#include "screen.h"
#include "mem.h"
#include "frame.h"
#include "kheap.h"

page_directory_t * current_directory = 0;

void set_addr_mapping(u32 start, u32 end, page_directory_t *pdir) {
    while (start < end) {
        page_t *pPage = get_page(start, 1, pdir);
        if (pPage == 0)
        {
            kprintf("WARNING, get page return 0\n");
            return;
        }
        alloc_frame(pPage, 0, 1);
        
        start += 0x1000;
    }
}
extern int int_count;
void init_memory() {
    int_count = 0;
    init_nofree_mem();

    u32 mem_end_page = 0x1000000;
    nframes = mem_end_page / 0x1000;

    page_directory_t *k_dir = (page_directory_t *)kmalloc_a_nofree(sizeof(page_directory_t), 1);
    kmemset((u8 *)k_dir, 0, sizeof(page_directory_t));

    current_directory = k_dir;

    frames = (u32 *)kmalloc_nofree(INDEX_FROM_BIT(nframes));
    kmemset((u8 *)frames, 0, INDEX_FROM_BIT(nframes));

    // first 2M mapping.
    set_addr_mapping(0, 0x00200000, k_dir);
    // C0000000 kernal heap 1M mapping.
    set_addr_mapping(KHEAP_VADDR_START, KHEAP_VADDR_START + KHEAP_INIT_SIZE, k_dir);

    register_interrupt_handler(14, page_fault);

    switch_page_directory(k_dir);

    init_dynamic_heap();
}
