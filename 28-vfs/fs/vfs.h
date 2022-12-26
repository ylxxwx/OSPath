#ifndef _VFS_H_
#define _VFS_H_

#include "disk.h"

int mount_root(disk_t *disk);
int mount(disk_t *disk, char *path);
int umount(disk_t *disk);
void ls_cur_dir();
void ls_dir(char *dir);
void cd_dir(char *path);
void more_file(char *path);
void vfs_read_file(char *path, u8* buf);
void vfs_pwd();

#endif //_VFS_H_