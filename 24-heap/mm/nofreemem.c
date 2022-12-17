#include "nofreemem.h"

/* This should be computed at link time, but a hardcoded
 * value is fine for now. Remember that our kernel starts
 * at 0x1000 as defined on the Makefile */
u32 free_mem_addr = 0x0000;

u32 kmalloc_nofree(u32 sz) {
    if (free_mem_addr + sz >= 0x7C00) {
        kprint("WARNING!!!! nofree memory reach 0x7C00\n");
        return 0;
    }

    if (free_mem_addr + sz >= 0xA0000) {
        kprint("WARNING!!!! nofree memory reach 0xA0000\n");
        return 0;
    }
    u32 tmp = free_mem_addr;
    free_mem_addr += sz;

    return tmp;
}

u32 kmalloc_a_nofree(u32 sz, int align) {    
    if (align == 1 && (free_mem_addr & 0xFFF)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }
    u32 tmp = kmalloc_nofree(sz);
    return tmp;
}

u32 kmalloc_ap_nofree(u32 sz, int align, u32 *phys) {
    u32 tmp = kmalloc_a_nofree(sz, align);
    *phys = tmp;
    return tmp;
}

void init_nofree_mem() {
    u32 *addr = (u32 *)0x0000;
    //free_mem_addr = *addr;
    kprint("NoFree memory start: ");
    kprint_hex(free_mem_addr);
    kprint("\n");
}
