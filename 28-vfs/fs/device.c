#include "device.h"
#include "utils.h"
#include "inode.h"
#include "disk.h"
#include "trace.h"
#include "fsmem.h"
#include "mem.h"
#include "screen.h"

device_t disks[2][2];

device_t *get_device(disk_t *disk) {
    device_t * ret = &(disks[disk->major][disk->minor]);
    return ret;
}

void init_dev(device_t *device) {
    if (0 == device)
       return;
    trace("=============init_dev==========\n");
    disk_t *disk = &device->disk;
    //show_disk("init_dev: ", disk);

    if(disk->major == 1) {
        device->partition_offset = read_partition_offset(disk);
        trace("partition offset:%d\n", device->partition_offset);
    } else {
        device->partition_offset = 0;
    }
    int ret = read_super_block(disk);
}

void show_super(super_block_t *super) {
    if (super) {
        trace("=====Super Block=====\n");
        trace("volume name:%s, uuid:%x-%x-%x-%x\n", super->s_volume_name, super->s_uuid[0],
            super->s_uuid[1],super->s_uuid[2],super->s_uuid[3]);
        trace("inode count:%d, avail inode:%d\n", super->s_inodes_count, super->s_free_inodes_count);
        trace("block count:%d, avail block:%u\n", super->s_blocks_count, super->s_free_blocks_count);
        trace("log :%d, first data block:%d\n", super->s_log_block_size, super->s_first_data_block);
        trace("magic num:%x, inode size: %d\n", super->s_magic, super->s_inode_size);
        trace("s_first_ino:%d, \n", super->s_first_ino);
    }
}

void show_desp(group_desc_t *desp) {
    if (desp) {
        trace("Block bitmap block:%d, inode bitmapblock:%d\n", desp->bg_block_bitmap, desp->bg_inode_bitmap);
        trace("Inode table block:%d, userd dir count:%d\n", desp->bg_inode_table, desp->bg_used_dirs_count);
        trace("free blocks count:%d, free inodes count:%d\n", desp->bg_free_blocks_count, desp->bg_free_inodes_count);
    }
}

void read_desp(device_t *device) {
    if (device->super->s_magic == 0xef53) {
        unsigned int group_count = 1 + (device->super->s_blocks_count-1) / device->super->s_blocks_per_group;
        trace("block id for super :%d, groups:%d\n", device->super->s_first_data_block,
          group_count);
        int block_id_for_desp = device->super->s_first_data_block +1;
        trace("!!!read_desp, block size:%d, desp size:%d\n", device->block_size, sizeof(group_desc_t));
        device->desp = (group_desc_t*)alloc_mem(device->block_size);
        read_blocks(&device->disk, block_id_for_desp, (u8*)device->desp);
        show_desp(device->desp);
        show_desp(device->desp + 1);
        show_desp(device->desp + 2);
        show_desp(device->desp + 3);
    }
}


typedef struct tag_par{
    u8 boot_signature;
    u8 start_head;
    u8 start_sector;
    u8 start_cylinder;
    u8 system_signature;
    u8 end_head;
    u8 end_sector;
    u8 end_cylinder;
    u32 nrb_before;
    u32 nrb_inside;
} partition_t;

int read_blocks(disk_t *disk, int start_block_id, u8* buffer) {
    device_t *device = get_device(disk);
    if (device->ready == 0) {
        device->disk = *disk;
        init_dev(device);
    }
    int sector_id = block_offset(device->partition_offset, start_block_id, device->super->s_log_block_size);
    trace("read_blocks: sector_id:%d\n", sector_id);
    
    int sz = read_sector(disk, sector_id, sectors_in_block(device->super->s_log_block_size), (u8*)buffer);
    return sz;
}

int read_partition_offset(disk_t *disk) {
    partition_t *ppt;

    u8 *buffer = (u8 *)kmalloc(BLOCK_SIZE);
    int sz = read_sector(disk, 0, 1, buffer);
    ppt = (partition_t *)(buffer + 0x1BE);

    for (int idx = 0; idx < 4; idx++) {
        if (ppt->boot_signature != 0x00 && ppt->boot_signature != 0x80) {
            stdoutput("hd partition table is not correct.\n");
            break;
        }
        if (ppt->system_signature != 0x83) {
            stdoutput("hd partition table is not Linux. idx:%d, %x\n", idx, ppt->system_signature);
            break;
        }
        if (disk->partition == idx)
            return ppt->nrb_before;
        ppt++;
    }
    kfree(buffer);
    return 0;
}

int read_super_block(disk_t *disk) {
    device_t *device = get_device(disk);
    int super_loc = superblock_offset(device->partition_offset);
    device->super = (super_block_t *)alloc_mem(2*BLOCK_SIZE);
    int sz = read_sector(disk, super_loc, 2, (u8*)device->super);

    //if (sz == 2) {
        show_super(device->super);
        device->ready = 1;
        device->block_size = 512 * sectors_in_block(device->super->s_log_block_size);

        read_desp(device);
    //}
    return 0;
}
