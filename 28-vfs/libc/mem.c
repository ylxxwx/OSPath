#include "mem.h"
#include "kheap.h"
#include "screen.h"

u32  kmalloc(u32 size) {
    dynamic_heap_alloc(size, 0);
}

u32  kmalloc_a(u32 size, int align) {
    return dynamic_heap_alloc(size, 1);
}

u32  kmalloc_ap(u32 size, int align, u32 *phy) {
    return dynamic_heap_alloc(size, 1);
}

void kfree(u8* addr) {
    dynamic_heap_free(addr);
}

void kmemcpy(u8 *dest, u8 *src, u32 nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(src + i);
    }
}

void kmemset(u8 *dest, u8 val, u32 len) {
    u8 *temp = (u8 *)dest;
    for ( ; len != 0; len--) {
        *temp++ = val;
    }
}
