#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "task.h"
#include "process.h"
#include "type.h"
#include "page.h"

typedef enum
{
    PROCESS_NORMAL,
    PROCESS_EXIT,
    PROCESS_EXIT_ZOMBIE
} process_status;

typedef struct pcb
{
    int id;
    char name[16];
    // int thread_id[4];
    struct pcb *parent;
    struct task_struct *threads[4];
    process_status status;
    // exit code
    int32 exit_code;
    int32 is_kernel_process;
    // page directory
    page_directory_t *page_dir;
} pcb_t;

pcb_t *create_process(char *name, int is_kernel_process);
void add_thread_to_process(pcb_t *process, struct task_struct *task);
void remove_thread_from_process(pcb_t *process, struct task_struct *task);
void show_process();
void clean_process();
void waitforpid(u32 pid);
int32 process_exec(char *path);
#endif // _PROCESS_H_
