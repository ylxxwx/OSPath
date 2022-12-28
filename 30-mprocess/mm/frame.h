#ifndef _FRAME_H_
#define _FRAME_H_

#include "page.h"

extern u32 *frames;
extern u32 nframes;

void alloc_frame(page_t *page, int is_kernel, int is_writeable);
void map_address(u32 vaddr, int is_kernel, int is_writable);
void init_frames(u32 mem_size);

#endif // _FRAME_H_
