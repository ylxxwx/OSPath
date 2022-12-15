#include "mem.h"
#include "../drivers/screen.h"

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
u32 free_mem_addr = 0x0000000;
/* Implementation is just a pointer to some free memory which
 * keeps growing */
#if 0
u32 kmalloc(u32 size, int align, u32 *phys_addr) {
    kprint("\n kmalloc: ");
    kprint_hex(size);

    /* Pages are aligned to 4K, or 0x1000 */
    if (align == 1 && (free_mem_addr & 0xFFFFF000)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }
    /* Save also the physical address */
    if (phys_addr) *phys_addr = free_mem_addr;

    u32 ret = free_mem_addr;
    free_mem_addr += size; /* Remember to increment the pointer */
    kprint(" addr: ");
    kprint_hex(ret);
    kprint("\n");
    return ret;
}
#endif

u32 kmalloc_a(u32 sz) {
/*    kprint("kmalloc_a: ");
    kprint("   free mem addr: ");
    kprint_hex(free_mem_addr);
    kprint("   size: ");
    kprint_hex(sz);
    kprint("\n");
*/
    u32 tmp = free_mem_addr;
    free_mem_addr += sz;

    return tmp;
}

u32 kmalloc_p(u32 sz, int align) {
    //kprint("\n kmalloc_p: ");
    //kprint_hex(free_mem_addr);
    
    if (align == 1 && (free_mem_addr & 0xFFF)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }
    u32 tmp = kmalloc_a(sz);
    //kprint(" addr: ");
    //kprint_hex(tmp);
    //kprint("\n");

    return tmp;
}

u32 kmalloc_ap(u32 sz, int align, u32 *phys) {
    //kprint("\n kmalloc_ap: ");
    //kprint_hex(sz);

    u32 tmp = kmalloc_p(sz, align);
    *phys = tmp;
    //kprint(" addr: ");
    //kprint_hex(tmp);
    //kprint("\n");

    return tmp;
}
