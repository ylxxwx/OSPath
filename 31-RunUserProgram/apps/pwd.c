#include "syscall_user.h"
#include "io.h"
#include "string.h"

void help()
{
    output("pwd             \n");
    output("  --help   show this help message.\n");
}

int main(int argc, char *argv[])
{
    for (int idx = 0; idx < argc; idx++)
        output(" argv[%d]:%s\n", idx, argv[idx]);
    // if (argc > 1)
    //     help();
    // else
    sys_pwd();
}
