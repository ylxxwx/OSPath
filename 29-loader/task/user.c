#include "user.h"
#include "syscall_user.h"
#include "shell.h"

void user_func() {
  //kprintf("user space entry...\n");
  //kprintf("mount root ret:%d\n", ret);

  shell();

  return;
}
