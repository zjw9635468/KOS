#ifndef CONSOLE_BUF_H
#define CONSOLE_BUF_H

#include <stdlib.h>

#include "simulator.h"
#include "scheduler.h"
#include "kos.h"

struct buffer{
    int value[257];
    int size;
    int head;
    int tail;
};

extern struct buffer *console_buffer;

// func
extern void *read_from_console();
#endif