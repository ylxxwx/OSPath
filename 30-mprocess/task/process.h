#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "type.h"
#include "page.h"
// #include "task.h"
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
    int thread_id[4];
    struct pcb *parent;
    // tcb_t *threads[4];
    process_status status;
    // exit code
    int32 exit_code;
    int32 is_kernel_process;
    // page directory
    page_directory_t *page_dir;
} pcb_t;

pcb_t *create_process(char *name, int is_kernel_process);

#endif // _PROCESS_H_
