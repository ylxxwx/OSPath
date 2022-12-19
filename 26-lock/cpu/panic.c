#include "../drivers/screen.h"

void panic(char *msg) {
    kprintf(msg);
    asm("hlt");
}