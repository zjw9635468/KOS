#include <stdlib.h>
#include "scheduler.h"

Dllist readyq;
struct PCB* currentRunningPCB;
JRB rbtree;
int curpid;
struct PCB* sentinel;
JRB file_opened;
int curfd;

void scheduler_init(){
    readyq= new_dllist();
    create_memory();
    // currentRunningPCB=(struct PCB*)malloc(sizeof(struct PCB));
}

void schedule(){
    kt_joinall();
    JRB ptr = jrb_first(sentinel->children);
    if(ptr == jrb_nil(sentinel->children)){
        SYSHalt();
    }
    
    //printf("default debug sched\n");
    if (dll_empty(readyq)){
        
        //printf("debug sched\n");
        is_noop=TRUE;
        noop();
    }else{
        //printf(" else if  statement debug sched\n");
        Dllist toDo=dll_first(readyq);
        currentRunningPCB=(struct PCB*)(toDo->val.v);
        dll_delete_node(toDo);
        is_noop=FALSE;
        User_Base=currentRunningPCB->pcb_base;
        User_Limit=currentRunningPCB->pcb_limit;
        run_user_code(currentRunningPCB->registers); 
    }

}

extern int perform_execve(struct PCB* pcb, char* filename, char** pcb_argv){
    //ToDo: error handling
    for (int i=0; i<NumTotalRegs; ++i){
        pcb->registers[i]=0;
    }
    pcb->registers[PCReg] = 0;
    pcb->registers[NextPCReg] = 4;
    pcb->pcb_base = User_Base;
    pcb->pcb_limit = User_Limit;
    pcb->waiter_sem=make_kt_sem(0);
    pcb->waiters=new_dllist();

   
	struct FD* std_in = malloc(sizeof(struct FD)); 
	struct FD* std_out = malloc(sizeof(struct FD));
    struct FD* std_err = malloc(sizeof(struct FD)); 

	std_in->console_indicator = 1;
	std_in->read_or_write = 0;
	std_in->offset = 0;
    
	
	std_out->console_indicator = 1;
	std_out->read_or_write = 1;
	std_out->offset = 0;
    

    std_err->console_indicator = 1;
	std_err->read_or_write = 1;
	std_err->offset = 0;
    

    // std_err->console_indicator = 1;
	// std_err->read_or_write = 1;
	// std_err->offset = 0;

	
	pcb->fd_table[0] = std_in;
	pcb->fd_table[1] = std_out;
	pcb->fd_table[2] = std_err;
	

	for(int i = 3; i < 64; i++){
		pcb->fd_table[i] = NULL;
	}

    int sbrk_ptr=load_user_program(filename);
    if (sbrk_ptr < 0) {
        fprintf(stderr,"Can't load program.\n");
        return(-EFAULT);
    }
    
    
    pcb->sbrk_ptr=(void*)sbrk_ptr;
    
    int size = 0;
    while(pcb_argv[size]){
        size++;
    }
    int tos, argv, k;
    int argvptr[256];
    int j;
    tos = pcb->pcb_limit - 12 - 1024;
    for(j = 0; j < size; j++){
        tos -= (strlen(pcb_argv[j]) + 1);
        while(tos%4 != 0){
            tos--;
        }
        argvptr[j] = tos;
        strcpy(main_memory+tos+pcb->pcb_base, pcb_argv[j]);
    }
    

    while (tos % 4) tos--;

    tos -= 4;
    k = 0;
    memcpy(main_memory+tos+pcb->pcb_base, &k, 4);

    for(int i = size - 1; i >= 0; i--){
        tos -= 4;
        memcpy(main_memory+tos+pcb->pcb_base, &argvptr[i], 4);
    }

    argv = tos;

    //envp
    tos -= 4;
    k = 0;
    memcpy(main_memory+tos+pcb->pcb_base, &k, 4);

    //&argv
    tos -= 4;
    memcpy(main_memory+tos+pcb->pcb_base, &argv, 4);

    //argc
    tos -= 4;
    k = size;
    memcpy(main_memory+tos+pcb->pcb_base, &k, 4);	

    tos -= 12;
    memset(main_memory+tos+pcb->pcb_base, 0, 12);

    /* need to back off from top of memory */
    /* 12 for argc, argv, envp */
    /* 12 for stack frame */
    pcb->registers[StackReg] = tos;

    return 0;
}

void *initialize_user_process(void *arg){
    curfd = 0;
    file_opened = make_jrb();

    start_timer(10);
    User_Base= 0;

    struct PCB* newPCB;
    bzero(main_memory, MemorySize);

    /* set up the program counters and the stack register */

    newPCB=(struct PCB*)malloc(sizeof(struct PCB));
    sentinel =(struct PCB*)malloc(sizeof(struct PCB));

    // even though sentinel does nothing, we still need to intialize it.
    sentinel->pid=get_new_pid();
    sentinel->waiter_sem=make_kt_sem(0);
    sentinel->waiters=new_dllist();
    sentinel->children = make_jrb();

    for (int i=0; i<NumTotalRegs; ++i){
        newPCB->registers[i]=0;
    }

    
    int wanted_pid = get_new_pid();
    newPCB->pid = (unsigned short)wanted_pid;

    memory_chunk[0]=1;
    newPCB->mem_loc=0;

    newPCB->parent=sentinel;
    newPCB->children = make_jrb();

    jrb_insert_int(sentinel->children, newPCB->pid, new_jval_v((void*)newPCB));
    
    int execv = perform_execve(newPCB, kos_argv[0], kos_argv);

    if(execv == 0){
        dll_append(readyq,new_jval_v((void*)newPCB));
    
        kt_exit();
    }else{
        exit(1);
    }
    
}

int get_new_pid(){
    curpid = 0;
    while(jrb_find_int(rbtree, curpid)){
        curpid++;
    }
    jrb_insert_int(rbtree, curpid, JNULL);
    return curpid;
}

void destroy_pid(int pid){
    JRB node = jrb_find_int(rbtree, pid);
    if(!node){
        return;
    }
    jrb_delete_node(node);
}

