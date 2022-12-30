#include "process.h"
#include "string.h"
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
    kmemset(process, 0, sizeof(pcb_t));

    uint32 id = find_avail_id();
    if (id == -1)
        panic("no more process resource.");
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
