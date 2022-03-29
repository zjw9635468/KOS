#ifndef KOS_H
#define KOS_H

#include <stdlib.h>


#include "simulator.h"
#include "scheduler.h"
#include "kt.h"
#include "syscall.h"
#include "console_buf.h"

#define BUFFERSIZE 256

// variables

extern kt_sem writelock;
extern kt_sem writer;
extern kt_sem reader;
extern kt_sem nelem;
extern kt_sem nslots;
extern kt_sem consoleWait;

extern bool is_noop;

/* boot the kos */
extern void boot();
#endif