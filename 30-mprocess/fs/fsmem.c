#include "type.h"
#include "fsmem.h"
#include "mem.h"
#include "string.h"

u8* alloc_mem(u32 size) {
    return (u8*)kmalloc(size);
}

void free_mem(u8* buffer) {
    kfree(buffer);
}

void clear_mem(u8* mem, u8 val, u32 size) {
    kmemset(mem, val, size);
}

char* move_str(u8* dst, u8 *src, u32 len) {
    kmemcpy(dst, src, len);
    return dst;
}

void mov_mem(u8* dst, u8 *src, u32 len) {
    kmemcpy(dst, src, len);
}

char cmp_str(u8* dst, u8* src, u32 len) {
    return strcmp(dst, src);
}