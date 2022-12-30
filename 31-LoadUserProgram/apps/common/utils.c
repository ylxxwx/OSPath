#include "type.h"
#include "utils.h"

void* get_ebp() {
    void* ret;
    __asm__ __volatile__ (
        "mov %%ebp, %0;"
        :"=m"(ret)
    );
    return ret;
}
