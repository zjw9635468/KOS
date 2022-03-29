/*
 * exception.c -- stub to handle user mode exceptions, including system calls
 * 
 * Everything else core dumps.
 * 
 * Copyright (c) 1992 The Regents of the University of California. All rights
 * reserved.  See copyright.h for copyright notice and limitation of
 * liability and disclaimer of warranty provisions.
 */
#include <stdlib.h>

#include "simulator.h"
#include "scheduler.h"
#include "kt.h"
#include "syscall.h"
#include "kos.h"

void
exceptionHandler(ExceptionType which)
{
	int             type, r5, r6, r7, newPC;
	// initialize the pcb to save the current process.
	//struct PCB *save_state=(struct PCB*)malloc(sizeof(struct PCB));
	// examine_registers(buf);
	// for (int i=0; i<NumTotalRegs; ++i){
	// 	currentRunningPCB->registers[i]=buf[i];
	// }
	if (is_noop==FALSE){
		examine_registers(currentRunningPCB->registers);
	}
	type = currentRunningPCB->registers[4];
	r5 = currentRunningPCB->registers[5];
	r6 = currentRunningPCB->registers[6];
	r7 = currentRunningPCB->registers[7];
	newPC = currentRunningPCB->registers[NextPCReg];

	/*
	 * for system calls type is in r4, arg1 is in r5, arg2 is in r6, and
	 * arg3 is in r7 put result in r2 and don't forget to increment the
	 * pc before returning!
	 */

	switch (which) {
	case SyscallException:
		/* the numbers for system calls is in <sys/syscall.h> */
		switch (type) {
		case SYS_halt:
			/* 0 is our halt system call number */
			DEBUG('e', "Halt initiated by user program\n");
			SYSHalt();
		case SYS_exit:
			/* this is the _exit() system call */
			DEBUG('e', "_exit() system call\n");
			// printf("Program exited with value %d.\n", r5);
			kt_fork(my_exit,(void *)currentRunningPCB);
			break;
		case SYS_write:
			/* this is the write() system call */
			DEBUG('e',"write() system call\n");
			kt_fork(do_write,(void *)currentRunningPCB);
			break;
		case SYS_read:
			/* this is the read() system call */
			DEBUG('e',"read() system call\n");
			kt_fork(do_read,(void *)currentRunningPCB);
			break;
		case SYS_ioctl:
			DEBUG('e',"ioctl() system call\n");
			kt_fork(ioctl,(void *)currentRunningPCB);
			break;
		case SYS_fstat:
			DEBUG('e',"fstat() system call\n");
			kt_fork(fstat,(void *)currentRunningPCB);
			break;
		case SYS_getpagesize:
			DEBUG('e',"getpagesize() system call\n");
			kt_fork(getpagesize,(void *)currentRunningPCB);
			break;
		case SYS_sbrk:
			DEBUG('e',"sbrk() system call\n");
			kt_fork(my_sbrk,(void *)currentRunningPCB);
			break;
		case SYS_execve:
			DEBUG('e', "execve() system call\n");
			kt_fork(my_execve, (void *)currentRunningPCB);
			break;
		case SYS_getpid:
			DEBUG('e', "getpid() system call\n");
			kt_fork(getpid, (void *)currentRunningPCB);
			break;
		case SYS_fork:
			DEBUG('e',"fork() system call\n");
			kt_fork(my_fork,(void *)currentRunningPCB);
			break;
		case SYS_getdtablesize:
			DEBUG('e',"getdtablesize() system call\n");
			kt_fork(getdtablesize,(void *)currentRunningPCB);
			break;
		case SYS_close:
			DEBUG('e',"close() system call\n");
			kt_fork(my_close,(void *)currentRunningPCB);
			break;
		case SYS_wait:
			DEBUG('e',"wait() system call\n");
			kt_fork(wait,(void *)currentRunningPCB);
			break;
		case SYS_getppid:
			DEBUG('e',"getppid() system call\n");
			kt_fork(getppid,(void *)currentRunningPCB);
			break;
		case SYS_dup:
			DEBUG('e',"dup() system call\n");
			kt_fork(my_dup,(void *)currentRunningPCB);
			break;
		case SYS_dup2:
			DEBUG('e',"dup2() system call\n");
			kt_fork(my_dup2,(void *)currentRunningPCB);
			break;
		case SYS_pipe:
			DEBUG('e',"pipe() system call\n");
			kt_fork(my_pipe,(void *)currentRunningPCB);
			break;
		default:
			DEBUG('e', "Unknown system call\n");
			SYSHalt();
			break;
		}
		break;
	case PageFaultException:
		DEBUG('e', "Exception PageFaultException\n");
		break;
	case BusErrorException:
		DEBUG('e', "Exception BusErrorException\n");
		break;
	case AddressErrorException:
		DEBUG('e', "Exception AddressErrorException\n");
		break;
	case OverflowException:
		DEBUG('e', "Exception OverflowException\n");
		break;
	case IllegalInstrException:
		DEBUG('e', "Exception IllegalInstrException\n");
		break;
	default:
		printf("Unexpected user mode exception %d %d\n", which, type);
		exit(1);
	}
	schedule();
}

void
interruptHandler(IntType which)
{
	// save the PCB states.
	if (is_noop==FALSE){
		// append to the scheduler queue.
		examine_registers(currentRunningPCB->registers);
		dll_append(readyq,new_jval_v((void *)currentRunningPCB));
	}
	

	// handle interrupts.
	switch (which) {
	case ConsoleReadInt:
		DEBUG('e', "ConsoleReadInt interrupt\n");
		V_kt_sem(consoleWait);
		break;
	case ConsoleWriteInt:
		DEBUG('e', "ConsoleWriteInt interrupt\n");
		V_kt_sem(writelock);
		break;
	case TimerInt:
		DEBUG('e', "TimerInt interrupt\n");
	
		break;
	default:
		DEBUG('e', "Unknown interrupt\n");
		break;
	}
	schedule();
}
