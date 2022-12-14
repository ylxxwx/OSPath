#include "../drivers/screen.h"

void panic(char *msg) {
    kprint(msg);
    asm("hlt");
}