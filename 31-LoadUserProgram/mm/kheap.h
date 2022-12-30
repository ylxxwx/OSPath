#ifndef _KHEAP_H_
#define _KHEAP_H_

#include "type.h"
#include "ordered_array.h"

#define KHEAP_VADDR_START 0xC0000000 // 3G
#define KHEAP_INIT_SIZE 0x300000     // 1M

typedef struct
{
  u32 magic;
  u8 is_hole;
  u32 size;
} header_t;

typedef struct
{
  u32 magic;
  header_t *header;
} footer_t;

typedef struct
{
  ordered_array_t index;
  u32 start_address;
  u32 end_address;
  u32 max_address;
  u32 supervisor;
  u8 readonly;
} heap_t;

heap_t *init_kheap();
u32 kheap_ready();

// heap_t *create_heap(u32 start, u32 end, u32 max, u8 supervisor, u8 readyonly);

void *heap_alloc(u32 size, u8 page_align, heap_t *heap);
void heap_free(void *p, heap_t *heap);

void init_dynamic_heap();
void *dynamic_heap_alloc(u32 size, u8 page_align);
void dynamic_heap_free(void *p);

#endif //_KHEAP_H_
