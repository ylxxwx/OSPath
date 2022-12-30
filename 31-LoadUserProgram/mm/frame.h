#ifndef _FRAME_H_
#define _FRAME_H_

#include "page.h"

extern u32 *frames;
extern u32 nframes;

void alloc_frame(page_t *page, int is_kernel, int is_writeable);
void map_address(u32 vaddr, int is_kernel, int is_writable);
void init_frames(u32 mem_size);
void set_frame(u32 frame_id);
void clear_frame(u32 frame_id);
u32 get_frame_user_count(u32 frame_id);

#endif // _FRAME_H_
