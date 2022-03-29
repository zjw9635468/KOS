#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "simulator.h"
#include "dllist.h"
#include "kt.h"
#include "kos.h"
#include "jrb.h"
#include "memory.h"

struct Pipe{
    char pipe_buffer[8193];
     // reference count
    int r_count;
    int w_count;

    kt_sem pipe_slots;
    kt_sem pipe_elem;
    kt_sem nwriter;
    kt_sem nreader;

    int head;
    int tail;
};

struct FD{
    int console_indicator;
    // 0: read, 1: write
    int read_or_write;
    int offset;
    
    struct Pipe* file_pipe;
};

struct PCB{
    int registers[NumTotalRegs];
    void* sbrk_ptr;
    int pcb_base;
    int pcb_limit;
    unsigned short pid;
    int exit_value;
    struct PCB* parent;
    int mem_loc;
    kt_sem waiter_sem;
    Dllist waiters;
    JRB children;
    struct FD* fd_table[64];
};

extern Dllist readyq;
extern struct PCB* currentRunningPCB;
extern int curpid;
extern JRB rbtree;
extern struct PCB* sentinel;

extern void scheduler_init();
extern void schedule();
extern void *initialize_user_process(void *arg);
extern int perform_execve(struct PCB* pcb, char* filename, char** argv);
extern int get_new_pid();
extern void destroy_pid(int pid);
// extern int check_memory_room(void *arg);



#endif