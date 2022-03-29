#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "simulator.h"
#include "scheduler.h"
#include "kos.h"


extern void syscall_return(struct PCB* pcb, int value);
extern void *do_write(void *arg);
extern void *do_read(void *arg);
extern void *ioctl(void *arg);
extern void *fstat(void *arg);
extern void *getpagesize(void *arg);
extern void *my_sbrk(void *arg);
extern void *my_execve(void *arg);
extern void *getpid(void *arg);
extern void *my_fork(void *arg);
extern void *finish_fork(void *arg);
extern void *my_exit(void *arg);
extern void *getdtablesize(void *arg);
extern void *my_close(void *arg);
extern void *wait(void *arg);
extern void *getppid(void *arg);

extern bool ValidAddress(void *arg);
extern bool has_empty_memory(int* ptr);

extern void *my_dup(void* arg);
extern void *my_dup2(void* arg);
extern void *my_pipe(void* arg);

extern int file_close(struct PCB* arg, int fd_index);

extern int endfile;

#endif