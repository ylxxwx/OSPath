#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "type.h"
#include "ext2.h"
#include "disk.h"

#define BLOCK_SIZE       512

#define NUM_BLOCK_BUFFER 10
#define PATH_DEPTH       16
typedef struct {
    u8 *buffer;
    u32 block_id;
} block_buffer;

typedef struct {
    disk_t disk;
    char *name;
    int ready;

    u32 partition_offset;
    super_block_t *super;
    group_desc_t  *desp;
    u32 block_size;
    u32           dir_entry_idx;
    block_buffer  dir_buffer;
    block_buffer  inode_table;
    block_buffer  data_buffer;
} device_t;

void init_dev(device_t *device);
device_t *get_device(disk_t *disk);

int read_blocks(disk_t *disk, int start_block_id, u8* buffer);
int read_partition_offset(disk_t *disk);
int read_super_block(disk_t *disk);
void read_desp(device_t *device);

#endif //_DEVICE_H_
