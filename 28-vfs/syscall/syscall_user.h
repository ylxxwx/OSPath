#ifndef _SYSCALL_USER_H_
#define _SYSCALL_USER_H_

#include "type.h"


s32 fork();
s32 exec(char* path, u32 argc, char* argv[]);
s32 sleep();
s32 user_mount_root(u32 major, u32 min, u32 partition);
s32 sys_print(char* fmt, void *arg);
s32 sys_std_input(char *buf);
s32 sys_clear_screen();
s32 sys_ls();
s32 sys_cd_dir(char *dir);
s32 sys_read_file(char *fn, char *buf);
#endif // _SYSCALL_USER_H_
