#ifndef _NOFREE_MEM_H_
#define _NOFREE_MEM_H_

#include "type.h"

void init_nofree_mem();
u32 kmalloc_nofree(u32 sz);
u32 kmalloc_a_nofree(u32 sz, int align);
u32 kmalloc_ap_nofree(u32 sz, int align, u32 *phys);

#endif
