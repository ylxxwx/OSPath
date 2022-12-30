#include "screen.h"
#include "task.h"
#include "process.h"
#include "string.h"
#include "mem.h"
#include "panic.h"

static pcb_t *processes[4];
static int cur_process = 0;

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
                    free_page_dir(processes[idx]->page_dir);
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