#include "frame.h"
#include "../cpu/type.h"
#include "memory.h"

/*
    frame bit(idx, offset) <--> physical address 1:1 mapping.
    virtual address <--> page 1:1 mapping
*/

u32 *frames;
u32 nframes;

extern u32 placement_address;

static void set_frame(u32 frame_addr) {
    u32 frame = frame_addr /0x1000;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

static void clear_frame(u32 frame_addr) {
    u32 frame = frame_addr / 0x1000;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

static u32 test_frame(u32 frame_addr) {
    u32 frame = frame_addr / 0x1000;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

static u32 first_frame() {
    u32 i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
        if (frames[i] != 0xFFFFFFFF) {
            for (j = 0; j < 32; j++) {
                u32 toTest = 0x1 << j;
                if ( !(frames[i]&toTest)) {
                    return i*4*8 + j;
                }
            }
        }
    }
    return 0;
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable) {
    if (page->frame != 0) {
        return;
    } else {
        u32 idx = first_frame();
        if (idx == (u32) -1) {
            panic("No Free frames!");
        }
        set_frame(idx*0x1000);
        page->present = 1;
        page->rw = (is_writeable)?1:0;
        page->user = (is_kernel)?0:1;
        page->frame = idx;
    }
}

void free_frame(page_t *page) {
    u32 frame;
    if (!(frame = page->frame)) {
        return;
    } else {
        clear_frame(frame);
        page->frame = 0x0;
    }
}