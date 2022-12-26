#ifndef _MEM_H_
#define _MEM_H_

#include "type.h"

u8* alloc_mem(u32 size);
void free_mem(u8*);
void clear_mem(u8* mem, u8 val, u32 size);
void mov_mem(u8* dst, u8 *src, u32 len);
char* move_str(u8* dst, u8 *src, u32 len);
char cmp_str(u8* dst, u8* src, u32 len);

#endif //_MEM_H_
