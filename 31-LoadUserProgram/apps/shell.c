#include "syscall_user.h"
#include "io.h"
#include "string.h"

#define CMD_CLS 0
#define CMD_LS 1
#define CMD_CD 2
#define CMD_MORE 3
#define CMD_EXIT 4
#define CMD_HELP 5
#define CMD_TRACEON 6
#define CMD_TRACEOFF 7
#define CMD_INVALID 8
#define CMD_FORK 10
#define CMD_TOP 11
#define CMD_FSIZE 12
#define CMD_CLS_TASK 13

void execCmd(int cmd, int argc, char argv[][80]);

void help(int cmd)
{
    switch (cmd)
    {
    case CMD_HELP:
        output("help            -- show all commands.\n");
        break;
    case CMD_LS:
        output("ls              -- show current directory.\n");
        break;
    case CMD_CD:
        output("cd [dir]        -- enter the next directory.\n");
        break;
    case CMD_MORE:
        output("more [file]     -- show file contents.\n");
        break;
    case CMD_EXIT:
        output("exit            -- exit this program.\n");
        break;
    case CMD_TRACEON:
        output("traceon         -- enable trace.\n");
        break;
    case CMD_TRACEOFF:
        output("traceoff        -- disable trace.\n");
        break;
    default:
        help(CMD_HELP);
        help(CMD_LS);
        help(CMD_CD);
        help(CMD_MORE);
        help(CMD_EXIT);
        help(CMD_TRACEON);
        help(CMD_TRACEOFF);
        break;
    }
}

const int str_2_cmd(char *str)
{
    int cmd = CMD_INVALID;
    if (0 == strcmp("clstask", str))
    {
        cmd = CMD_CLS_TASK;
    }
    else if (0 == strcmp("size", str))
    {
        cmd = CMD_FSIZE;
    }
    else if (0 == strcmp("top", str))
    {
        cmd = CMD_TOP;
    }
    else if (0 == strcmp("cls", str))
    {
        cmd = CMD_CLS;
    }
    else if (0 == strcmp("ls", str))
    {
        cmd = CMD_LS;
    }
    else if (0 == strcmp("cd", str))
    {
        cmd = CMD_CD;
    }
    else if (0 == strcmp("more", str))
    {
        cmd = CMD_MORE;
    }
    else if (0 == strcmp("exit", str))
    {
        cmd = CMD_EXIT;
    }
    else if (0 == strcmp("help", str))
    {
        cmd = CMD_HELP;
    }
    else if (0 == strcmp("traceon", str))
    {
        cmd = CMD_TRACEON;
    }
    else if (0 == strcmp("traceoff", str))
    {
        cmd = CMD_TRACEOFF;
    }
    else if (0 == strcmp("fork", str))
    {
        cmd = CMD_FORK;
    }
    return cmd;
}

int main(int argc, char *argv[])
{
    u8 buf[1024];
    s32 ret = 0;
    char *prompt = "> ";
    while (1)
    {
        output(prompt);
        memset((u8 *)buf, 0, 1024);

        int ret = input(buf);

        if (strlen(buf) == 0 || buf[0] == '\n')
        {
            continue;
        }

        cmd_t CMD;
        memset(&CMD, 0, sizeof(CMD));
        int num = split(CMD.str, buf, " ");
        // int num = split(CMD.argv, buf, " ");
        CMD.argc = num;
        for (int idx = 0; idx < 16; idx++)
        {
            CMD.argv[idx] = &CMD.str[idx][0];
        }
        if (num == 0)
        {
            output("%s, split 0.\n", buf);
            continue;
        }
        int cmd = str_2_cmd(CMD.argv[0] /*str[0]*/);
        if (cmd == CMD_INVALID)
        {
            int ret = sys_fork();
            // output("out of fork: ret:%d\n", ret);
            if (ret == 0)
            {
                // while (1)
                //{
                sys_exec(CMD.argv[0], &CMD);
                sys_exit();
                //}
            }
            sys_waitforpid(ret);
        }
        else
        {
            execCmd(cmd, num, CMD.argv);
        }
    }
}

void execCmd(int cmd, int argc, char argv[][80])
{
    switch (cmd)
    {
    case CMD_CLS:
        std_clear_screen();
        break;
    case CMD_LS:
        if (argc == 1)
        {
            argv[1][0] = '.';
            argv[1][1] = '\0';
        }
        sys_ls(argv[1]);
        break;
    case CMD_CD:
    {
        if (argc == 2)
        {
            sys_cd_dir(argv[1]);
        }
        else
        {
            output("input num:%d, expect 2.\n", argc);
        }
        break;
    }
    case CMD_MORE:
    {
        if (argc != 2)
        {
            output("input num:%d, expect 2.\n", argc);
            return;
        }
        else
        {
            // int sz = sys_fsize(argv[1]);
            // output("file:%s, size:%d\n", argv[1], sz);

            u8 buf[2048];
            memset(buf, 0, 2048);
            int sz = sys_read_file(argv[1], buf);
            output("file:%s, size:%d\n", argv[1], sz);
            output(buf);
        }
        break;
    }
    case CMD_TRACEON:
    {
        // enable_trace();
        break;
    }
    case CMD_TRACEOFF:
    {
        // disable_trace();
        break;
    }
    case CMD_HELP:
    {
        help(CMD_INVALID);
        break;
    }
    case CMD_FORK:
    {
        int ret = sys_fork();
        output("out of fork: ret:%d\n", ret);
        if (ret == 0)
        {
            while (1)
            {
                sys_ls(".");
                sys_ls("data1");
                sys_exit();
                while (1)
                    sys_sleep();
                output("out of sleep: v1 :%d:\n", ret);
            }
        }

        sys_waitforpid(ret);

        break;
    }
    case CMD_TOP:
    {
        sys_top();
        break;
    }
    case CMD_FSIZE:
    {
        if (argc != 2)
        {
            output("input num:%d, expect 2.\n", argc);
            return;
        }
        else
        {
            int sz = sys_fsize(argv[1]);
            output("file:%s, size:%d\n", argv[1], sz);
        }
        break;
    }
    case CMD_CLS_TASK:
    {
        sys_cls_task();
        break;
    }
    case CMD_INVALID:
    case CMD_EXIT:
    {
        return;
    }
    default:
    {
        break;
    }
    }
}
