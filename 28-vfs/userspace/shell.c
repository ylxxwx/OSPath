#include "shell.h"
#include "io.h"
#include "mem.h"
#include "string.h"

void shell() {
  u8 buf[256];
  while(1) {
    output("> ");
    kmemset(buf, 0, 256);
    int ret = input(buf);
    //int ret = sleep();
    output("input: %s\n", buf);
    if (0 == strcmp("CLEAR", buf)) {
        std_clear_screen();
    }
    //kprintf("ret val of sleep:%d\n", ret);
  }
}