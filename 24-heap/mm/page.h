#ifndef _PAGE_H_
#define _PAGE_H_

#include "memory.h"

extern page_directory_t * current_directory;

page_t *get_page(u32 address, int make, page_directory_t *dir);

#endif //_PAGE_H_
