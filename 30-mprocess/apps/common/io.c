#include "type.h"
#include "io.h"
#include "utils.h"
#include "syscall_user.h"

s32 input(u8 * buf) {
    return sys_std_input(buf);
}

s32 output(char *fmt,...) {
  void *ebp = 0;
  __asm__ __volatile__ (
      "mov %%ebp, %0;"
      :"=m"(ebp)
  );
  void *arg_ptr = (void *)(ebp + 12);
  return sys_print(fmt, arg_ptr);
}

s32 std_clear_screen() {
  return sys_clear_screen();
}