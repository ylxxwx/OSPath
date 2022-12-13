#include "mem.h"

void memory_copy(u8 *source, u8 *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void kmemset(u8 *dest, u8 val, u32 len) {
    u8 *temp = (u8 *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

/* This should be computed at link time, but a hardcoded
 * value is fine for now. Remember that our kernel starts
 * at 0x1000 as defined on the Makefile */
u32 free_mem_addr = 0x10000;
/* Implementation is just a pointer to some free memory which
 * keeps growing */
u32 kmalloc(u32 size, int align, u32 *phys_addr) {
    /* Pages are aligned to 4K, or 0x1000 */
    if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }
    /* Save also the physical address */
    if (phys_addr) *phys_addr = free_mem_addr;

    u32 ret = free_mem_addr;
    free_mem_addr += size; /* Remember to increment the pointer */
    return ret;
}

u32 kmalloc_a(u32 sz) {
    //kprint_hex(0xABCDEF01);
    //kprint("kmalloc_a placement_address:  ");
    //kprint_hex(placement_address);
    //kprint("\n sz: ");
    //kprint_hex(sz);
    //kprint("\n");
    u32 tmp = free_mem_addr;
    free_mem_addr += sz;
    return tmp;
}

u32 kmalloc_p(u32 sz, int align) {
    if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }
    return kmalloc_a(0x1000);
}

u32 kmalloc_ap(u32 sz, int align, u32 *phys) {
    u32 tmp = kmalloc_p(sz, align);
    *phys = tmp;
    return tmp;
}
