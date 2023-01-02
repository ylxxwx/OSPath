#include "screen.h"
#include "task.h"
#include "process.h"
#include "string.h"
#include "mem.h"
#include "panic.h"
#include "schedule.h"

static pcb_t *processes[4];
static int cur_process = 0;
void waitforpid(u32 pid)
{
    while (processes[pid] != 0)
    {
        do_context_switch();
    }
}

int find_avail_id()
{
    for (int idx = 0; idx < 4; idx++)
    {
        if (processes[idx] == 0)
            return idx;
    }
    return -1;
}

pcb_t *create_process(char *name, int is_kernel_process)
{
    pcb_t *process = (pcb_t *)kmalloc(sizeof(pcb_t));
    kmemset((u8 *)process, 0, sizeof(pcb_t));

    uint32 id = find_avail_id();
    if (id == -1)
        panic("no more process resource.");
    processes[id] = process;
    process->id = id;
    // if (name != NULL)
    //{
    //     kstrcpy(process->name, name);
    // }
    //  else {
    //  char buf[16];
    //  sprintf(buf, "process-%u", process->id);
    //  kstrcpy(process->name, buf);
    // }
    process->parent = NULL;

    process->status = PROCESS_NORMAL;
    process->is_kernel_process = is_kernel_process;
    process->exit_code = 0;
    process->page_dir = clone_crt_page_dir(current_directory);
    // kprintf("process dir:va:%x pa:%x\n", process->page_dir->tablesPhysical, vaddr_to_paddr(process->page_dir->tablesPhysical));
    return process;
}

void add_thread_to_process(pcb_t *process, tcb_t *task)
{
    for (int idx = 0; idx < 4; idx++)
    {
        if (process->threads[idx] == 0)
        {
            process->threads[idx] = task;
            return;
        }
    }
    panic("no room for this task in process.");
}

void remove_thread_from_process(pcb_t *process, tcb_t *task)
{
    for (int idx = 0; idx < 4; idx++)
    {
        if (process->threads[idx] == task)
        {
            process->threads[idx] = 0;
            return;
        }
    }
}

void clean_process()
{
    for (int idx = 0; idx < 4; idx++)
    {
        if (processes[idx] != 0)
        {
            int thread_count = 0;
            for (int j = 0; j < 4; j++)
            {
                if (processes[idx]->threads[j] != 0)
                {
                    thread_count++;
                }
            }
            if (thread_count == 0)
            {
                if (0 != processes[idx]->page_dir)
                {
                    free_page_dir(processes[idx]->page_dir);
                }
                kfree(processes[idx]);
                processes[idx] = 0;
            }
        }
    }
}

void show_process()
{
    kprintf("process id         thread\n");
    for (int idx = 0; idx < 4; idx++)
    {
        if (processes[idx] != 0)
        {
            kprintf("%d          ", idx);
            for (int j = 0; j < 4; j++)
            {
                if (processes[idx]->threads[j] != 0)
                {
                    kprintf("  %x", processes[idx]->threads[j]->id);
                }
            }
            kprintln();
        }
    }
}

int32 process_exec(char *path, cmd_t *cmd)
{
    // TODO: disallow exec if there are multiple threads running on this process?
    cmd_t save_cmd;
    kmemcpy((u8 *)&save_cmd, cmd, sizeof(cmd_t));

    u32 path_len = strlen(path);

    char *buf = (char *)kmalloc(path_len + 6);
    char *target_path = buf + 5;
    kmemset(buf, 0, path_len + 6);
    kmemcpy(buf, "/bin/", 5);
    kmemcpy(buf + 5, path, strlen(path));
    //     Check target file exist.
    int fsize = file_size(target_path);
    if (fsize <= 0)
    {
        target_path = buf;
        fsize = file_size(target_path);
        if (fsize <= 0)
        {
            kprintf("process_exec can't find the file.\n");
            kfree(buf);
            return 0;
        }
    }
    //   Find cur thread.
    tcb_t *crt_thread = get_cur_thread();
    pcb_t *process = crt_thread->process;

    free_userspace_page(current_directory);
    switch_page_directory(current_directory);

    if (vfs_read_file(target_path, (u8 *)0xA0000000) != fsize)
    {
        kprintf("cmd %s not found.\n", path);
        // kfree(read_buffer);
        kfree(buf);
        return -1;
    }

    cmd_t *target_cmd = (cmd_t *)((u8 *)USER_PARAM_TOP - sizeof(cmd_t));
    kmemcpy((u8 *)target_cmd, (u8 *)&save_cmd, sizeof(cmd_t));
    for (int idx = 0; idx < 16; idx++)
    {
        if (save_cmd.argv[idx] != 0)
        {
            target_cmd->argv[idx] = target_cmd->str[idx];
        }
    }
    // panic("free save cmd.");
    //  Create a new thread to exec new program.
    tcb_t *thread = create_user_thread(process, "asdb", (void *)0xA0000000, 3);

    disable_interrupt();
    mov_task_ready(thread->id);
    while (1)
    {
        mov_cur_task_dead();
        do_context_switch();
    }
}
