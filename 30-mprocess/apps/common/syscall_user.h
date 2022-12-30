#ifndef _SYSCALL_USER_H_
#define _SYSCALL_USER_H_

#include "type.h"

s32 sys_fork();
s32 sys_sleep();
s32 sys_pwd();
s32 sys_cls();
s32 sys_input(u32 buf_addr /*char *buf*/);
s32 sys_ls(u32 dir_addr /* char **/);
s32 sys_cd_dir(char *dir);
s32 sys_print(char *fmt, void *arg);
s32 sys_read_file(char *fn, char *buf);
s32 sys_exec(char *path, u32 argc, char *argv[]);
s32 sys_mount_root(u32 major, u32 min, u32 partition);

#endif // _SYSCALL_USER_H_
