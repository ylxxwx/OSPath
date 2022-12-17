#include "kheap.h"
#include "screen.h"
#include "ordered_array.h"

// 24 C 1100  26
// 1G 40000000 28
// 2G 80000000 31
// 3G C0000000 
#define KHEAP_MAX_SIZE 0x40000000
#define KHEAP_INDEX_SIZE 0x200
#define KHEAP_MAGIC 0x123890AB
#define KHEAP_HEAD_SIZE (sizeof(header_t))
#define KHEAP_FOOT_SIZE (sizeof(footer_t))

u32 kheapReady = 0;
heap_t *dynamicHeap = 0;
u32  kheap_ready() {
    return kheapReady;
}

/*
header_t *debug = 0;
void show_debug(char *str) {
    kprint(str);
    kprint(" block head: ");
    kprint_hex(debug);
    kprint(" block magic: ");
    kprint_hex(debug->magic);
    kprint(" block size:");
    kprint_hex(debug->size);
    kprint("\n");
}
*/

static s8 header_t_less_than(void *a, void *b) {
    return ((header_t*)a) < ((header_t*)b)? 1:0;
}

heap_t *create_heap(u32 start, u32 end, u32 max, u8 supervisor, u8 readonly) {
    heap_t *heap = (header_t*) kmalloc_nofree(sizeof(heap_t));
    void *buf = kmalloc_nofree(sizeof(u32) *KHEAP_INDEX_SIZE);
    if (buf == 0) {
        kprint("kmalloc memory for heap failed.\n");
        return 0;
    }

    heap->index = place_ordered_array((void*)buf, KHEAP_INDEX_SIZE, &header_t_less_than);
    //start += KHEAP_INDEX_SIZE;
    //if (start & 0xFFFFF000) {
    //    start &= 0xFFFFF000;
    //    start += 0x1000;
    //}

    heap->start_address = start;
    heap->end_address = end;
    heap->max_address = max;
    heap->supervisor = supervisor;
    heap->readonly = readonly;

    header_t *hole = (header_t*)start;
//debug = hole;
    hole->size = end - start;
    hole->magic = KHEAP_MAGIC;
    hole->is_hole = 1;
    footer_t *footer = (footer_t *)(start + hole->size - KHEAP_FOOT_SIZE);
    footer->magic = KHEAP_MAGIC;
    footer->header = hole;
    insert_ordered_array((void*)hole, &heap->index);
    return heap;
}

s32 find_smallest_hole(u32 size, u8 page_align, heap_t *heap) {
    u32 iterator = 0;
    while (iterator < heap->index.size) {
        header_t *header = (header_t *) lookup_ordered_array(iterator, &heap->index);
        if (page_align > 0) {
            u32 location = (u32)header + (u32)KHEAP_HEAD_SIZE;
            u32 offset = 0;
            if ((u32)(location & 0xFFF) != 0) {
                offset = (u32)0x1000 - (u32)location&0xFFF;
            }
            u32 hole_size = (u32)header->size - (u32)offset;
            if (hole_size >= size)
                break;
        } else if (header->size >= size) {
            break;
        }
        iterator++;
    }
    if (iterator == heap->index.size) {
        return -1;
    }
    return iterator;
}


void *heap_alloc(u32 size, u8 page_align, heap_t *heap) {
    u32 new_size = size + sizeof(header_t) + sizeof(footer_t);
    s32 iterator = find_smallest_hole(new_size, page_align, heap);
    if (iterator == -1) {
        kprint("heap_alloc failed. Run out of memory.\n");
        return 0;
    }

    header_t* orig_hole_header = (header_t*) lookup_ordered_array(iterator, &heap->index);

    u32 orig_hole_pos = (u32) orig_hole_header;
    u32 orig_hole_size = orig_hole_header->size;
    
    // can't split to two holes.
    if (orig_hole_size <= new_size + sizeof(header_t) + sizeof(footer_t)) {
        size = orig_hole_size - sizeof(header_t) - sizeof(footer_t);
        new_size = orig_hole_size;
    }

    // align
    u32 orig_data_pos = orig_hole_pos + sizeof(header_t);
    if (page_align && orig_data_pos&0xFFF) {
        u32 new_location = (u32)((orig_data_pos + 0x1000)&0xFFFFF000) - KHEAP_HEAD_SIZE;
        header_t* hole_header = (header_t*)orig_hole_pos;
        hole_header->size = new_location - orig_hole_pos;
        footer_t* hole_footer = (footer_t*) ((u32)new_location - sizeof(footer_t));
        hole_footer->header = hole_header;
        hole_footer->magic = KHEAP_MAGIC;
        orig_hole_pos = new_location;
        orig_hole_size = orig_hole_size - hole_header->size;
    } else {
        remove_ordered_array(iterator, &heap->index);
    }
    header_t *block_header = (header_t*)orig_hole_pos;
    block_header->magic = KHEAP_MAGIC;
    block_header->is_hole = 0;
    block_header->size = new_size;
    footer_t *block_foot = (footer_t *)(orig_hole_pos + sizeof(header_t) + size);
    block_foot->header = block_header;
    block_foot->magic = KHEAP_MAGIC;

    if (orig_hole_size - new_size > 0) {
        header_t *hole_header = (header_t *)((u32)block_header + block_header->size);        
        hole_header->magic = KHEAP_MAGIC;
        hole_header->is_hole = 1;
        hole_header->size = orig_hole_size - new_size;
        footer_t *hole_footer = (footer_t *)((u32) hole_header + hole_header->size - sizeof(footer_t)); 

        if ((u32)hole_footer < heap->end_address) {
            hole_footer->magic = KHEAP_MAGIC;
            hole_footer->header = hole_header;
        }
        insert_ordered_array((void*)hole_header, &heap->index);
    }
    return (void *)((u32)block_header + KHEAP_HEAD_SIZE);
}

void heap_free(void* p, heap_t *heap) {
    if (p == 0)
        return;

    header_t *header = (header_t *)((u32)p - KHEAP_HEAD_SIZE);
    footer_t *footer = (footer_t *)((u32)header + header->size - KHEAP_FOOT_SIZE);

    if (header->magic != KHEAP_MAGIC) {
        kprint("heap_free, but header magic is not correct.\n");
        return;
    }
    if (footer->magic != KHEAP_MAGIC) {
        kprint("heap_free, but footer magic is not correct.\n");
        return;
    }

    header->is_hole = 1;
    char do_add = 1;

    if ((u32)header <= heap->start_address) {
        kprint("kheap free. First block. head <= start addreee.\n");
    } else {
        footer_t *prev_foot = (footer_t *)((u32)header - KHEAP_FOOT_SIZE);
        if (prev_foot->magic == KHEAP_MAGIC && prev_foot->header->is_hole == 1) {
            u32 cache_size = header->size;
            header = prev_foot->header;
            header->size += cache_size;
            do_add = 0;
        } else {
            kprint("kheap_free prev hole Addr : ");
            kprint_hex(prev_foot->header);
            kprint(" magic: ");
            kprint_hex(prev_foot->header->magic);
            kprint("\n");
        }
    }

    if ((u32)footer + KHEAP_FOOT_SIZE >= heap->end_address) {
        // do_add == 0 means attached to prev hole, need to remove prev hole.
        if (do_add == 0) {
            u32 iterator = 0;
            while ((iterator < heap->index.size) && ((void *)header != lookup_ordered_array(iterator, &heap->index)))
                iterator++;
            if (iterator >= heap->index.size) {
                kprint("heap_free didn't find next in array.\n");
            } else {
                remove_ordered_array(iterator, &heap->index);
            }
        } else {
            // do nothing.
        }
    } else {
        header_t *next_head = (header_t *) (footer + KHEAP_FOOT_SIZE);
        if (next_head->magic == KHEAP_MAGIC && next_head->is_hole) {
            footer = (footer_t *)(next_head + next_head->size - KHEAP_FOOT_SIZE);
            footer->header = header;
            u32 iterator = 0;
            while ((iterator < heap->index.size) && 
                   ((type_t)(next_head) != lookup_ordered_array(iterator, &heap->index)))
                iterator++;
            if (iterator >= heap->index.size) {
                kprint("WARNING!!!heap_free didn't find next in array.\n");
                return;
            }
            remove_ordered_array(iterator, &heap->index);
        }
    }

    if (do_add == 1) {
        insert_ordered_array((void*)header, &heap->index);
    }
}

heap_t *init_kheap() {
    heap_t *heap = create_heap(KHEAP_VADDR_START, KHEAP_VADDR_START + KHEAP_INIT_SIZE, KHEAP_MAX_SIZE, 0, 0);

    kheapReady = 1;
    return heap;    
}

void init_dynamic_heap() {
    dynamicHeap = init_kheap(KHEAP_VADDR_START, KHEAP_VADDR_START + KHEAP_INIT_SIZE, KHEAP_MAX_SIZE, 0, 0);
}

void *dynamic_heap_alloc(u32 size, u8 page_align) {
    if (dynamicHeap) {
       return heap_alloc(size, page_align, dynamicHeap);
    }
    return 0;
}

void dynamic_heap_free(void* p) {
    if (dynamicHeap) {
        heap_free(p, dynamicHeap);
    }
}
