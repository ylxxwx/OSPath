#include "frame.h"
#include "type.h"
#include "memory.h"
#include "panic.h"

/*
    virtual address <--> page 1:1 mapping
*/

u32 nframes;
u8 *frame_user_count;

extern u32 placement_address;

void init_frames(u32 mem_size)
{
    nframes = mem_size / 0x1000;
    frame_user_count = (u32 *)kmalloc_nofree(sizeof(u8) * nframes);
    kmemset((u8 *)frame_user_count, 0, sizeof(u8) * nframes);
}

void set_frame(u32 frame_id)
{
    frame_user_count[frame_id]++;
    // kprintf("up frame count:%d, %d\n", frame_id, frame_user_count[frame_id]);
}

void clear_frame(u32 frame_id)
{
    frame_user_count[frame_id]--;
    // kprintf("down frame count:%d, %d\n", frame_id, frame_user_count[frame_id]);
}

u32 get_frame_user_count(u32 frame_id)
{
    return frame_user_count[frame_id];
}

static u32 test_frame(u32 frame_addr)
{
    u32 frame = frame_addr / 0x1000;
    return frame_user_count[frame] > 0;
}

static u32 first_frame()
{
    for (int idx = 0; idx < nframes; idx++)
    {
        if (frame_user_count[idx] == 0)
            return idx;
    }
    panic("Can't found free frame.");
    return 0;
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
    if (page->frame != 0)
    {
        return;
    }
    else
    {
        u32 idx = first_frame();
        set_frame(idx);
        page->present = 1;
        page->rw = (is_writeable) ? 1 : 0;
        page->user = (is_kernel) ? 0 : 1;
        page->frame = idx;
    }
}

void free_frame(page_t *page)
{
    u32 frame;
    if (!(frame = page->frame))
    {
        return;
    }
    else
    {
        clear_frame(frame);
        page->frame = 0x0;
    }
}

void map_address(u32 vaddr, int is_kernel, int is_writable)
{
    page_t *page = get_page(vaddr, 1, current_directory);
    if (page == 0)
    {
        panic("Get page failed");
        return;
    }
    alloc_frame(page, is_kernel, is_writable);
}

/*
void map_addr_phy(u32 vaddr, u32 paddr, int is_kernel, int is_writable) {
  page_t *page = get_page(vaddr, 1, current_directory);
  if (page == 0) {
    panic("Get page failed");
    return;
  }
  if (page->frame != 0) {
    panic("try to map a vaddr to a paddr, but there is a mapping.");
  } else {
        u32 idx = paddr/0x1000;
        set_frame(idx);
        page->present = 1;
        page->rw = (is_writable)?1:0;
        page->user = (is_kernel)?0:1;
        page->frame = idx;
    }
}
*/
