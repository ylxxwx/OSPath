#ifndef MEM_H
#define MEM_H

#include "type.h"

u32  kmalloc(u32 size);
u32  kmalloc_a(u32 size, int align);
u32  kmalloc_ap(u32 size, int align, u32 *phy);
void kfree(u8* addr);
void kmemcpy(u8 *dest, u8 *src, u32 nbytes);
void kmemset(u8 *dest, u8 val, u32 len);

#endif
