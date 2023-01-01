#include "syscall_user.h"
#include "io.h"
extern int main();

int __start__()
{
    main();
    sys_exit();
}
