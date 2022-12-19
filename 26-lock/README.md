*Concepts you may want to Google beforehand: malloc*

**Goal: Implement a memory allocator**

We will add a kernel memory allocator to `libc/mem.c`and `mem/nofree_mem.c`. It is 
implemented as a simple pointer to free memory, which keeps
growing.

There are two set of memory management interfaces.

One set consists of kmalloc/kmalloc_a/kmalloc_ap, it manages the memory from 0x00000000 to 0xA00000.
And these memory is not able to be freed. This set of memory management is located in mem/nofree_mem.c.

Another set of memory management is located in lib/mem.c, which manages memory virtual address from 0xc0000000 to 0xFFFFFFFF. This is a dynamic heap, can be freed.

u32  kmalloc(u32 size);
u32  kmalloc_a(u32 size, int align);
u32  kmalloc_ap(u32 size, int align, u32 *phy);

And in this task, the gdt tabel is moved to address 0x00000000, so we can get full use of address till 0xA00000.
