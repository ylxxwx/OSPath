#ifndef _INODE_H_
#define _INODE_H_
#include "disk.h"
#include "device.h"
#include "ext2.h"

#define INVALID_MODE 0xFFFF

inode_t *read_inode(device_t *dev, int inode_id);

u32 get_inode_mode(disk_t *dev, int inode_id);
int is_inode_file(disk_t *dev, int inode_id);
int is_inode_dir(disk_t *dev, int inode_id);

void show_inode(int id, inode_t *inode);

int get_next_dir_entry(disk_t *dev, int inode_id, char *name);
void reset_dir_entry(disk_t *disk, int inode_id);

int get_inode_file(disk_t *disk, int inode_id, char *buf);
u32 get_inode_fsize(disk_t *disk, int inode_id);
#endif //_INODE_H_
