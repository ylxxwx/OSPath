#ifndef _UTILS_H_
#define _UTILS_H_

#include "type.h"

/*
block start from 0.
boot sector locates in block 0, sector 0.
super (two sectors) locates in block 0/1(depends), sector 3(fixed)
desp locates in the next block to super
*/

u32 sectors_in_block(int block_sz);
u32 bootloader_offset();

/*offset per sector*/
u32 superblock_offset(int patition_offset);

/*offset per sector*/
u32 block_offset(int patition_offset,int block_id, int log_block_sz);

#endif // _UTILS_H_
