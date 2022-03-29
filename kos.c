/*
 * kos.c -- starting point for student's os.
 * 
 */

#include "kos.h"


// remain problem: handle the malloc every time you created.


kt_sem writelock;
kt_sem writer;
kt_sem reader;
kt_sem nelem;
kt_sem nslots;		// block when buffer is full
kt_sem consoleWait;	// block when character is being read.	
bool is_noop;
struct buffer *console_buffer;
static char *Argv[5] = { "argtest", "Rex,", "my", "man!", NULL };

void boot(){
	scheduler_init();
	writelock=make_kt_sem(0);
	writer=make_kt_sem(1);
	reader=make_kt_sem(1);
	consoleWait=make_kt_sem(0);
	nslots=make_kt_sem(BUFFERSIZE);
	nelem=make_kt_sem(0);
	is_noop=TRUE;
	console_buffer=(struct buffer *)malloc(sizeof(struct buffer));
	memset(console_buffer,0,sizeof(struct buffer));
	console_buffer->size=BUFFERSIZE+1;
	rbtree = make_jrb();
}

void KOS()
{
	boot();
	kt_fork(initialize_user_process, (void*)NULL);

	
	kt_fork(read_from_console,(void*)NULL);
	schedule();
}
