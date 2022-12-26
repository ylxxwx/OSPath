#include "user.h"
#include "syscall_user.h"
#include "shell.h"

void user_func() {
  //kprintf("user space entry...\n");
  //s32 ret = mount_root(4,5,6);
  //kprintf("mount root ret:%d\n", ret);

  shell();

  return;
}
