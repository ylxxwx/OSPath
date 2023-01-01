#include "syscall_user.h"
#include "io.h"

#define USER_PARAM_TOP 0xC0000000

extern int main(int argc, char *argv[]);

int __start__()
{
    cmd_t *params = (cmd_t *)((u8 *)USER_PARAM_TOP - sizeof(cmd_t));
    main(params->argc, params->argv);
    sys_exit();
}
