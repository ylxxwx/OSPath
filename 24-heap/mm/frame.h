#ifndef _FRAME_H_
#define _FRAME_H_

#include "page.h"

void alloc_frame(page_t *page, int is_kernel, int is_writeable);

#endif // _FRAME_H_
