#ifndef _SYSCALL_USER_H_
#define _SYSCALL_USER_H_

#include "type.h"

s32 sys_fork();
s32 sys_exec(char *path, u32 argc, char *argv[]);
s32 sys_sleep();
s32 sys_mount_root(u32 major, u32 min, u32 partition);
s32 sys_print(char *fmt, void *arg);
s32 sys_std_input(char *buf);
s32 sys_clear_screen();
s32 sys_ls(char *dir);
s32 sys_cd_dir(char *dir);
s32 sys_read_file(char *fn, char *buf);
s32 sys_pwd();
s32 sys_exit();
s32 sys_top();
s32 sys_fsize(char *path_addr);
s32 sys_cls_task();
s32 sys_waitforpid(u32 pid);
s32 sys_execute(char *path);

#endif // _SYSCALL_USER_H_
