// #include <stdlib.h>
// #include "syscall.h"

// int endfile = 0;

// bool ValidAddress(void *arg){
//     struct PCB *pcb = (struct PCB*)arg;

//     if (pcb->registers[RetAddrReg]>=0 && pcb->registers[RetAddrReg]<=pcb->pcb_limit){
//         return TRUE;
//     }
//     return FALSE;
// }

// bool has_empty_memory(int* ptr){
//     for (int i=0; i<sizeof(memory_chunk)/sizeof(int); ++i){
//         if (memory_chunk[i]==0){
//             *ptr=i;
//             return TRUE;
//         }
//     }
//     return FALSE;
// }

// void syscall_return(struct PCB* pcb, int value){
//     pcb->registers[PCReg]=pcb->registers[NextPCReg];
//     pcb->registers[2]=value;
//     dll_append(readyq,new_jval_v((void *)pcb));
//     // if (pcb->registers[4]==SYS_write){
//     //     V_kt_sem(writer);
//     // }
//     // if (pcb->registers[4]==SYS_read){
//     //     V_kt_sem(reader);
//     // }
//     kt_exit();
// }

// void *do_write(void *arg){
    
//     struct PCB *pcb=(struct PCB*)arg;
//     int fd_index = pcb->registers[5];
//     if(ValidAddress(arg) == FALSE){
//         syscall_return(pcb, -EFAULT);
//     }
//     if (fd_index >= 64 || fd_index < 0 || pcb->fd_table[fd_index] == NULL 
//         || pcb->fd_table[fd_index]->read_or_write != 1){
//         syscall_return(pcb,-EBADF);
//     }
//     if(pcb->fd_table[fd_index]->console_indicator == 0 && pcb->fd_table[fd_index]->file_pipe->r_count == 0){
//         syscall_return(pcb,-EPIPE);
//     }
//     if (pcb->registers[6]<0 || pcb->registers[6] >= pcb->pcb_limit){
//         syscall_return(pcb,-EFAULT);
//     }
//     if (pcb->registers[6] + pcb->registers[7] > pcb->pcb_limit-24){
//         syscall_return(pcb,-EFBIG);
//     }
//     if (pcb->registers[7]<0){
//         syscall_return(pcb,-EINVAL);
//     }
//     // init a lock
    
//     int i=0;
//     if(pcb->fd_table[fd_index]->console_indicator == 1){
//         P_kt_sem(writer);
//         while (i<pcb->registers[7]){        // size
//             // read from main memory
//             int input= pcb->registers[6];
//             char tmp= main_memory[input+i+pcb->pcb_base];

//             // write to the console
//             console_write(tmp);

//             // lock
//             P_kt_sem(writelock);
//             i++;
//         }
//         V_kt_sem(writer);
//     }else{
//         // printf("-------write into pipe buffer------\n");
//         if(kt_getval(pcb->fd_table[fd_index]->file_pipe->pipe_slots) == 0 ){
//             // V_kt_sem(pcb->fd_table[fd_index]->file_pipe->nwriter);
//             syscall_return(pcb,-EPIPE);
//         }
//         struct Pipe* cur_pipe = pcb->fd_table[fd_index]->file_pipe;
//         P_kt_sem(pcb->fd_table[fd_index]->file_pipe->nwriter);
//         while (i<pcb->registers[7]){        // size
            
//             // int pipe_write_end = pcb->fd_table[fd_index]->head;
            
//             // int input= pcb->registers[6];
//             // pcb->fd_table[fd_index]->file_pipe->pipe_buffer[pipe_write_end] = main_memory[input+i+pcb->pcb_base];
//             // // printf("%c",pcb->fd_table[fd_index]->file_pipe->pipe_buffer[pipe_write_end]);
//             // P_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_slots);
//             // i++;
//             // if (pcb->fd_table[fd_index]->file_pipe->r_count == 0) {
//             //     // printf("break write\n");
//             //     break;
//             // }
//             // pcb->fd_table[fd_index]->head = (pcb->fd_table[fd_index]->head+1)%8192;
//             // V_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_elem);
            
//             P_kt_sem(cur_pipe->pipe_slots);
//             if(cur_pipe->r_count == 0){
//                 syscall_return(pcb,-EBADF);
//             }
//             char temp = main_memory[pcb->registers[6]+i+pcb->pcb_base];
//             int next = (cur_pipe->head + 1)%8193;
//             cur_pipe->pipe_buffer[next] = temp;
//             cur_pipe->head = next;
//             V_kt_sem(cur_pipe->pipe_elem);
//             i++;

//         }
//         V_kt_sem(pcb->fd_table[fd_index]->file_pipe->nwriter);
//         // printf("\n--------end-----\n");
//     }
    
//     syscall_return(pcb,i);
// }
// void *do_read(void *arg){
    
//     struct PCB *pcb=(struct PCB*)arg;
//     int fd_index = pcb->registers[5];
//     if(ValidAddress(arg) == FALSE){
//         syscall_return(pcb, -EFAULT);
//     }
//     if(endfile == 1 && pcb->fd_table[fd_index]->console_indicator == 1){
//         syscall_return(pcb, 0);
//     }
//     if(fd_index < 0 || fd_index >= 64 || pcb->fd_table[fd_index] == NULL || pcb->fd_table[fd_index]->read_or_write == 1){
//         syscall_return(pcb,-EBADF);
//     }
//     if(pcb->registers[6]<0 || pcb->registers[6] >= pcb->pcb_limit){
//         syscall_return(pcb,-EFAULT);
//     }
//     if(pcb->registers[6] + pcb->registers[7] > pcb->pcb_limit - 24){
//         syscall_return(pcb, -EFBIG);
//     }
//     if(pcb->registers[7]<0){
//         syscall_return(pcb,-EINVAL);
//     }
//     if(pcb->fd_table[fd_index]->file_pipe){
//         if(pcb->fd_table[fd_index]->console_indicator == 0 && pcb->fd_table[fd_index]->file_pipe->w_count == 0 
//             && kt_getval(pcb->fd_table[fd_index]->file_pipe->pipe_elem) <=  0){
//             syscall_return(pcb, 0);
//         }
//     }
    
//     int i=0;
//     if(pcb->fd_table[fd_index]->console_indicator == 1){
//         P_kt_sem(reader);
//         i = 0;
//         while (i<pcb->registers[7]){        // size
//             // call this nelem to check there is available character to buffer.
//             // printf("pcb->registers[6]: %d\n", pcb->registers[6]);
//             P_kt_sem(nelem);
//             int next=(console_buffer->tail+1)%console_buffer->size;
//             int cur_char=console_buffer->value[next];
//             console_buffer->tail=next;
//             if (cur_char==-1){
//                 // ctrl D then EOF, we break the loop.
//                 if (endfile==0){
//                         endfile=1;
//                 }
//                 break;
//             }
            
//             main_memory[pcb->registers[6]+i+pcb->pcb_base]=cur_char;
//             // printf("%d: \n", pcb->registers[6] + i + pcb->pcb_base);
//             // printf("%c\n", main_memory[pcb->registers[6]+i+pcb->pcb_base]);
//             V_kt_sem(nslots);
//             i++;
//         }
//         V_kt_sem(reader);
//     }else{
//         P_kt_sem(pcb->fd_table[fd_index]->file_pipe->nreader);
//         i = 0;
//         // P_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_elem);
//         // V_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_elem);
//         int pipe_elem_val = kt_getval(pcb->fd_table[fd_index]->file_pipe->pipe_elem);
//         int size;
//         if(pipe_elem_val < pcb->registers[7]){
//             size = pipe_elem_val;
//         }else{
//             size = pcb->registers[7];
//         }
//         // printf("-------read from pipe buffer------\n");
//         while(i < size){
//             int pipe_read_end = pcb->fd_table[fd_index]->file_pipe->tail;
//             P_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_elem);
//             if (pcb->fd_table[fd_index]->file_pipe->w_count == 0) {
//                 i = 0;
//                 if(endfile == 0){
//                     endfile = 1;
//                 }
//                 // printf("break read\n");
//                 break;
//             } 
//             main_memory[pcb->registers[6]+i+pcb->pcb_base] = pcb->fd_table[fd_index]->file_pipe->pipe_buffer[pipe_read_end];
//             // printf("%c",pcb->fd_table[fd_index]->file_pipe->pipe_buffer[pipe_read_end]);
//             i++;
//             pcb->fd_table[fd_index]->file_pipe->tail = (pcb->fd_table[fd_index]->file_pipe->tail+1)%8193;
//             V_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_slots);
//         }
//         V_kt_sem(pcb->fd_table[fd_index]->file_pipe->nreader);
//         // printf("\n-------end------\n");
        
//     }
    
//     syscall_return(pcb,i);
// }

// void *ioctl(void *arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     if(ValidAddress(arg) == FALSE){
//         syscall_return(pcb, -EFAULT);
//     }
//     if (pcb->registers[5]!=1 || pcb->registers[6]!=JOS_TCGETP){
//         syscall_return(pcb,-EINVAL);
//     }
//     // maybe bug
    
//     struct JOStermios *input= (struct JOStermios*)&main_memory[pcb->registers[7]+pcb->pcb_base];
//     ioctl_console_fill(input);
//     syscall_return(pcb,0);
// }

// void *fstat(void *arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     int buff_size=0;
//     int fd_index = pcb->registers[5];
    
//     if(ValidAddress(arg) == FALSE){
//         syscall_return(pcb, -EFAULT);
//     }

//     if(fd_index >= 64 && fd_index < 0){
//         syscall_return(pcb, -EBADF);
//     }

//     if (pcb->registers[5]==0){
//         buff_size=1;
//     }else if (pcb->registers[5]==1 || pcb->registers[5]==2){
//         buff_size=256;
//     }else{
//         buff_size=8192;
//     }
//     struct KOSstat *input=(struct KOSstat*)&main_memory[pcb->registers[6]+pcb->pcb_base];

//     stat_buf_fill(input,buff_size);
//     syscall_return(pcb,0);
// }


// void *getpagesize(void *arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     syscall_return(pcb,PageSize);
// }

// void *my_sbrk(void *arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     if(ValidAddress(arg) == FALSE){
//         syscall_return(pcb, -EFAULT);
//     }

//     int increm=pcb->registers[5];
//     if (pcb->sbrk_ptr==NULL){
//         pcb->sbrk_ptr=0;
//     }
//     int prev_brk=(int)pcb->sbrk_ptr;
//     // handle later
//     if (prev_brk+increm>User_Limit){
//         syscall_return(pcb,-ENOMEM);
//     }
//     pcb->sbrk_ptr+=increm;
//     syscall_return(pcb,prev_brk);
// }

// void *my_execve(void *arg){
//     struct PCB *pcb=(struct PCB*)arg;

//     int str_size = 0;
//     while(main_memory[pcb->registers[5]+pcb->pcb_base+str_size]){
//         str_size++;
//     }
//     // printf("first loop finished\n");
//     char* filename = (char *)malloc((str_size+1)*sizeof(char));
//     for(int i = 0; i < str_size; i++){
//         strncpy(filename+i, main_memory+pcb->registers[5]+pcb->pcb_base+i, 1);
//     }
//     filename[str_size] = '\0';

//     int num_str=0;
//     int ptr=pcb->registers[6]+pcb->pcb_base;
//     while (TRUE){
//         int addr= main_memory[ptr] +
//         (main_memory[ptr+1]<<8)+
//         (main_memory[ptr+2]<<16)+
//         (main_memory[ptr+3]<<24);

//         if (addr == 0 || addr > pcb->pcb_base+User_Limit){
//             break;
//         }
//         num_str++;
//         ptr+=4;
//     }
//     // printf("second loop finished\n");
//     char** argv = (char**)malloc((num_str+1) * sizeof(char*));
//     // ptr = pcb->registers[6]+pcb->pcb_base;
//     // int comp= main_memory[ptr] +
//     //     (main_memory[ptr+1]<<8)+
//     //     (main_memory[ptr+2]<<16)+
//     //     (main_memory[ptr+3]<<24);
    
    

//     // handle the case if there is only one argument
//     // if (main_memory[comp] != main_memory[pcb->registers[5] + pcb->pcb_base]){
//     //     int size=0;
//     //     while (main_memory[pcb->registers[5]+pcb->pcb_base+size]!='\0'){
//     //         size++;
//     //     }
//     //     char * temp= (char*)malloc((size+1)*sizeof(char));
//     //     for (int j=0; j<size; ++j){
//     //         strncpy(temp+j,main_memory+pcb->registers[5]+pcb->pcb_base+j, 1);
//     //     }
//     //     argv[0]= (char*)malloc((size+1)*sizeof(char));
//     //     temp[size]='\0';
//     //     memcpy(argv[0], temp, size+1);
//     //     free(temp);
//     //     // printf("%s\n",temp);
//     // }else{
//     ptr=pcb->registers[6]+pcb->pcb_base;
//     for (int i=0; i<num_str; ++i){
//         int size=0;
//         int addr= main_memory[ptr] +
//         (main_memory[ptr+1]<<8)+
//         (main_memory[ptr+2]<<16)+
//         (main_memory[ptr+3]<<24);

//         char* c_ptr = main_memory+addr+pcb->pcb_base;
//         size = strlen(c_ptr);
        
//         // while (main_memory[addr+pcb->pcb_base+size]!='\0' && main_memory[addr+pcb->pcb_base+size]!=' '){
//         //     size++;
//         // }

//         char * temp= (char*)malloc((size+1)*sizeof(char));
//         for (int j=0; j<size; ++j){
            
//             memcpy(temp+j,main_memory+pcb->pcb_base+addr+j,1);
//         }
//         temp[size]='\0';
//         argv[i] = (char*)malloc((size+1)*sizeof(char));
//         memcpy(argv[i],temp, size+1);
//         ptr+=4;
//         free(temp);
//     }
//     // }
    
//     argv[num_str]=NULL;
    
    
//     int return_value = perform_execve(pcb, filename, argv);
//     pcb->registers[2]=return_value;
//     dll_append(readyq,new_jval_v((void *)pcb));
//     for(int i = 0; i< num_str; i++){
//         free(argv[i]);    
//     }
//     free(argv);
//     free(filename);
//     kt_exit();
// }

// void *getpid(void *arg){
//    struct PCB *pcb=(struct PCB*)arg;
//    syscall_return(pcb, pcb->pid); 
// }

// void *my_fork(void *arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     int mem_loc=0;
//     if (!has_empty_memory(&mem_loc)){
//         syscall_return(pcb,-EAGAIN);
//     }
//     struct PCB *newproc= (struct PCB*)malloc(sizeof(struct PCB));
    
//     // initialize and set all the fields for new process.
//     newproc->pid=get_new_pid();
//     newproc->pcb_base=mem_loc*User_Limit;
//     newproc->pcb_limit=User_Limit;
//     newproc->sbrk_ptr=pcb->sbrk_ptr;
//     newproc->mem_loc=mem_loc;
//     memory_chunk[newproc->mem_loc]=1;
//     newproc->parent=pcb;
//     newproc->waiter_sem=make_kt_sem(0);
//     newproc->waiters=new_dllist();
//     newproc->children = make_jrb();

//     jrb_insert_int(pcb->children, newproc->pid, new_jval_v((void*)newproc));

//     // copy the registers.
//     for (int i=0; i<NumTotalRegs; ++i){
//         newproc->registers[i]=pcb->registers[i];
//     }

//     for(int i = 0; i < 64; i++){
//         newproc->fd_table[i] = pcb->fd_table[i];
//         if(pcb->fd_table[i] != NULL){
//             if(pcb->fd_table[i]->console_indicator == 0 && pcb->fd_table[i]->read_or_write == 0){
//                 pcb->fd_table[i]->file_pipe->r_count ++;
//             }else if(pcb->fd_table[i]->console_indicator == 0 && pcb->fd_table[i]->read_or_write == 1){
//                 pcb->fd_table[i]->file_pipe->w_count ++;
//             }
//         }
//     }

//     // memory copy the from the current process.
//     memcpy(main_memory+newproc->pcb_base,main_memory+pcb->pcb_base,User_Limit);

//     // fork the new process
//     kt_fork(finish_fork,(void *)newproc);
//     syscall_return(pcb,newproc->pid);

// }


// void *finish_fork(void *arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     syscall_return(pcb,0);
// }

// void *my_exit(void *arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     pcb->exit_value=pcb->registers[5];
//     memory_chunk[pcb->mem_loc]=0;
//     memset(main_memory+pcb->pcb_base, 0, pcb->pcb_limit);
//     // for(int i = 0; i < pcb->pcb_limit; i++){
//     //     if(main_memory[pcb->pcb_base + i] != 0){
//     //         printf("%d: %c ",i, main_memory[pcb->pcb_base+i]);
//     //     }
//     // }
//     // printf("\n");
//     // dll_append(pcb->parent->waiters,new_jval_v((void*)pcb));
//     // V_kt_sem(pcb->parent->waiter_sem);

//     for(int i = 0; i < 64; i++){
//         if(pcb->fd_table[i] != NULL){
//             file_close(pcb, i);
//         }
//     }

//     jrb_delete_node(jrb_find_int(pcb->parent->children, pcb->pid));

//     JRB ptr;
//     jrb_traverse(ptr, pcb->children){
//         jrb_insert_int(sentinel->children, jval_i(ptr->key), ptr->val);
//         struct PCB* temp = (struct PCB*)(ptr->val.v);
//         temp->parent = sentinel;
//     }
//     jrb_free_tree(pcb->children);

//     if(!dll_empty(pcb->waiters)){
//         Dllist ptr;
//         dll_traverse(ptr, pcb->waiters){
//             struct PCB* child=(struct PCB*)(ptr->val.v);
//             dll_delete_node(ptr);
//             destroy_pid(child->pid);
//             for (int i=0; i<NumTotalRegs; ++i){
//                 child->registers[i]=0;
//             }
//             free(child);
//         }
//     }

    
    
//     // printf("here we are\n");
//     if(pcb->parent == sentinel){
//         destroy_pid(pcb->pid);
//         for (int i=0; i<NumTotalRegs; ++i){
//             pcb->registers[i]=0;
//         }
//         free(pcb);
//     }else{
//         V_kt_sem(pcb->parent->waiter_sem);
//         dll_append(pcb->parent->waiters,new_jval_v((void*)pcb));
        
//     }
//     kt_exit();
// }

// void *getdtablesize(void *arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     syscall_return(pcb,64);
// }

// void *my_close(void *arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     int fd_index = pcb->registers[5];
//     if(fd_index >= 64 || fd_index < 0 || pcb->fd_table[fd_index] == NULL){
//         syscall_return(pcb, -EBADF);
//     }
//     file_close(pcb, fd_index);
//     syscall_return(pcb,0);
// }

// void *wait(void *arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     JRB ptr = jrb_first(pcb->children);
//     if(ptr == jrb_nil(pcb->children) && dll_empty(pcb->waiters)){
//         syscall_return(pcb, -1);
//     }
//     P_kt_sem(pcb->waiter_sem);
//     Dllist toDo=dll_first(pcb->waiters);
//     struct PCB* child=(struct PCB*)(toDo->val.v);
    
//     int ret=child->pid;
//     memcpy(&(main_memory[pcb->registers[5]+ pcb->pcb_base]),&(child->exit_value),sizeof(int));
//     dll_delete_node(toDo);
//     destroy_pid(child->pid);
//     for (int i=0; i<NumTotalRegs; ++i){
//         child->registers[i]=0;
//     }
//     free(child);
//     syscall_return(pcb,ret);
// }

// void *getppid(void *arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     syscall_return(pcb,pcb->parent->pid);
// }

// void *my_dup(void* arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     int old_fd = pcb->registers[5];
//     if(pcb->fd_table[old_fd] == NULL){
//         syscall_return(pcb, -EBADF);
//     }
//     int new_fd = 0;
//     while(pcb->fd_table[new_fd] != NULL && new_fd < 64){
//         new_fd++;
//     }
//     if(new_fd == 64){
//         syscall_return(pcb, -EMFILE);
//     }
//     pcb->fd_table[new_fd] = pcb->fd_table[old_fd];
//     if(pcb->fd_table[old_fd]->file_pipe){
//         if(pcb->fd_table[old_fd]->read_or_write == 0){
//             pcb->fd_table[old_fd]->file_pipe->r_count++;
//         }else if(pcb->fd_table[old_fd]->read_or_write == 1){
//             pcb->fd_table[old_fd]->file_pipe->w_count++;
//         }
//     }
    

//     syscall_return(pcb, new_fd);

    
// }

// void *my_dup2(void* arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     int old_fd = pcb->registers[5];
//     if(pcb->fd_table[old_fd] == NULL){
//         syscall_return(pcb, -EBADF);
//     }
//     int new_fd = pcb->registers[6];

//     if(old_fd == new_fd){
//         syscall_return(pcb,new_fd);
//     }
    
//     if(new_fd >= 64 || new_fd < 0){
//         syscall_return(pcb, -EBADF);
//     }

//     if(pcb->fd_table[new_fd] != NULL){
//         file_close(pcb, new_fd);
//     }

//     pcb->fd_table[new_fd] = pcb->fd_table[old_fd];
//     if(pcb->fd_table[old_fd]->file_pipe){
//         if(pcb->fd_table[old_fd]->read_or_write == 0){
//             pcb->fd_table[old_fd]->file_pipe->r_count++;
//         }else if(pcb->fd_table[old_fd]->read_or_write == 1){
//             pcb->fd_table[old_fd]->file_pipe->w_count++;
//         }
//     }
    

//     syscall_return(pcb, new_fd);
// }

// void *my_pipe(void* arg){
//     struct PCB *pcb=(struct PCB*)arg;
//     if(!ValidAddress(arg)){
//         syscall_return(pcb, -EFAULT);
//     }
//     int pipe_ptr = pcb->registers[5];
//     int* pipe_desc = (int*) (&(main_memory[pipe_ptr+pcb->pcb_base]));

//     struct FD* fd_read = malloc(sizeof(struct FD));
//     struct FD* fd_write = malloc(sizeof(struct FD));
//     struct Pipe* ptr = malloc(sizeof(struct Pipe));

    

//     int read_end = 0;
//     while(pcb->fd_table[read_end] != NULL && read_end < 64){
//         read_end++;
//     }
//     if(read_end == 64){
//         syscall_return(pcb, -EMFILE);
//     }

//     int write_end = read_end + 1;
//     while(pcb->fd_table[write_end] != NULL && write_end < 64){
//         write_end++;
//     }
//     if(write_end == 64){
//         syscall_return(pcb, -EMFILE);
//     }

//     ptr->pipe_slots = make_kt_sem(8192);
//     ptr->pipe_elem = make_kt_sem(0);
//     ptr->nwriter = make_kt_sem(1);
//     ptr->nreader = make_kt_sem(1);
//     ptr->head = 0;
//     ptr->tail = 0;

//     pcb->fd_table[read_end] = fd_read;
//     pcb->fd_table[write_end] = fd_write;
    
//     ptr->r_count = 1;
//     ptr->w_count = 1;
//     memset(ptr->pipe_buffer, 0, 8192);

  
//     fd_read->console_indicator = 0;
//     fd_read->read_or_write = 0;
//     fd_read->offset = 0;
    
//     fd_read->file_pipe = ptr;

//     fd_write->console_indicator = 0;
//     fd_write->read_or_write = 1;
//     fd_write->offset = 0;
    
//     fd_write->file_pipe = ptr;

    

//     pipe_desc[0] = read_end;
//     pipe_desc[1] = write_end;

//     syscall_return(pcb, 0);
// }

// int file_close(struct PCB* arg, int fd_index){
//     if(fd_index >= 64 || fd_index < 0){
//         return EBADF;
//     }
//     struct FD *fd = arg->fd_table[fd_index];
//     if(fd->console_indicator == 1){
//         arg->fd_table[fd_index] = NULL;
//         // fd->console_indicator = 0;
//         return 0;
//     }
    
//     if(fd->read_or_write == 0 && fd->console_indicator == 0){
//         fd->file_pipe->r_count --;
//     }else if(fd->read_or_write == 1 && fd->console_indicator == 0){
//         fd->file_pipe->w_count --;
//     }
    
//     // if(fd->file_pipe->r_count == 0 || fd->file_pipe->w_count == 0){
        
//     // }

//     if (fd->file_pipe->r_count == 0 && fd->read_or_write == 0) {
//         V_kt_sem(fd->file_pipe->pipe_slots);
//     }

//     if (fd->file_pipe->w_count == 0 && fd->read_or_write == 1) {
//         while(fd->file_pipe->r_count > 0){
//             V_kt_sem(fd->file_pipe->pipe_elem);
//             fd->file_pipe->r_count --;
//         }
//     }
//     arg->fd_table[fd_index] = NULL;
//     return 0;
// }


#include <stdlib.h>
#include "syscall.h"

int endfile = 0;

bool ValidAddress(void *arg){
    struct PCB *pcb = (struct PCB*)arg;

    if (pcb->registers[RetAddrReg]>=0 && pcb->registers[RetAddrReg]<=pcb->pcb_limit){
        return TRUE;
    }
    return FALSE;
}

bool has_empty_memory(int* ptr){
    for (int i=0; i<sizeof(memory_chunk)/sizeof(int); ++i){
        if (memory_chunk[i]==0){
            *ptr=i;
            return TRUE;
        }
    }
    return FALSE;
}

void syscall_return(struct PCB* pcb, int value){
    pcb->registers[PCReg]=pcb->registers[NextPCReg];
    pcb->registers[2]=value;
    dll_append(readyq,new_jval_v((void *)pcb));
    // if (pcb->registers[4]==SYS_write){
    //     V_kt_sem(writer);
    // }
    // if (pcb->registers[4]==SYS_read){
    //     V_kt_sem(reader);
    // }
    kt_exit();
}

void *do_write(void *arg){
    
    struct PCB *pcb=(struct PCB*)arg;
    int fd_index = pcb->registers[5];
    if(ValidAddress(arg) == FALSE){
        syscall_return(pcb, -EFAULT);
    }
    if (fd_index >= 64 || fd_index < 0 || pcb->fd_table[fd_index] == NULL 
        || pcb->fd_table[fd_index]->read_or_write != 1){
        syscall_return(pcb,-EBADF);
    }
    if(pcb->fd_table[fd_index]->console_indicator == 0 && pcb->fd_table[fd_index]->file_pipe->r_count == 0){
        syscall_return(pcb,-EPIPE);
    }
    if (pcb->registers[6]<0 || pcb->registers[6] >= pcb->pcb_limit){
        syscall_return(pcb,-EFAULT);
    }
    if (pcb->registers[6] + pcb->registers[7] > pcb->pcb_limit-24){
        syscall_return(pcb,-EFBIG);
    }
    if (pcb->registers[7]<0){
        syscall_return(pcb,-EINVAL);
    }
    // init a lock
    
    int i=0;
    if(pcb->fd_table[fd_index]->console_indicator == 1){
        P_kt_sem(writer);
        while (i<pcb->registers[7]){        // size
            // read from main memory
            int input= pcb->registers[6];
            char tmp= main_memory[input+i+pcb->pcb_base];

            // write to the console
            console_write(tmp);

            // lock
            P_kt_sem(writelock);
            i++;
        }
        V_kt_sem(writer);
    }else{
        // printf("-------write into pipe buffer------\n");
        if(kt_getval(pcb->fd_table[fd_index]->file_pipe->pipe_slots) == 0 ){
            // V_kt_sem(pcb->fd_table[fd_index]->file_pipe->nwriter);
            syscall_return(pcb,-EPIPE);
        }
        
        P_kt_sem(pcb->fd_table[fd_index]->file_pipe->nwriter);
        while (i<pcb->registers[7]){        // size
            
            int pipe_write_end = pcb->fd_table[fd_index]->file_pipe->head;
            
            int input= pcb->registers[6];
            pcb->fd_table[fd_index]->file_pipe->pipe_buffer[pipe_write_end] = main_memory[input+i+pcb->pcb_base];
            // printf("%c",pcb->fd_table[fd_index]->file_pipe->pipe_buffer[pipe_write_end]);
            P_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_slots);
            // if(kt_getval(pcb->fd_table[fd_index]->file_pipe->pipe_slots) == 0 ){
            //     V_kt_sem(pcb->fd_table[fd_index]->file_pipe->nwriter);
            //     syscall_return(pcb, -EPIPE);
            // }
            i++;
            if (pcb->fd_table[fd_index]->file_pipe->r_count == 0) {
                // printf("break write\n");
                break;
            }
            pcb->fd_table[fd_index]->file_pipe->head = (pcb->fd_table[fd_index]->file_pipe->head+1)%8192;
            V_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_elem);
        }
        V_kt_sem(pcb->fd_table[fd_index]->file_pipe->nwriter);
        // printf("\n--------end-----\n");
    }
    
    syscall_return(pcb,i);
}
void *do_read(void *arg){
    
    struct PCB *pcb=(struct PCB*)arg;
    int fd_index = pcb->registers[5];
    if(ValidAddress(arg) == FALSE){
        syscall_return(pcb, -EFAULT);
    }
    if(endfile == 1 && pcb->fd_table[fd_index]->console_indicator == 1){
        // endfile = 0;
        syscall_return(pcb, 0);
    }
    if(fd_index < 0 || fd_index >= 64 || pcb->fd_table[fd_index] == NULL || pcb->fd_table[fd_index]->read_or_write == 1){
        syscall_return(pcb,-EBADF);
    }
    if(pcb->registers[6]<0 || pcb->registers[6] >= pcb->pcb_limit){
        syscall_return(pcb,-EFAULT);
    }
    if(pcb->registers[6] + pcb->registers[7] > pcb->pcb_limit - 24){
        syscall_return(pcb, -EFBIG);
    }
    if(pcb->registers[7]<0){
        syscall_return(pcb,-EINVAL);
    }
    if(pcb->fd_table[fd_index]->file_pipe){
        if(pcb->fd_table[fd_index]->console_indicator == 0 && pcb->fd_table[fd_index]->file_pipe->w_count == 0 
            && kt_getval(pcb->fd_table[fd_index]->file_pipe->pipe_elem) <=  0){
            syscall_return(pcb, 0);
        }
    }
    
    int i=0;
    if(pcb->fd_table[fd_index]->console_indicator == 1){
        P_kt_sem(reader);
        i = 0;
        while (i<pcb->registers[7]){        // size
            // call this nelem to check there is available character to buffer.
            // printf("pcb->registers[6]: %d\n", pcb->registers[6]);
            P_kt_sem(nelem);
            int next=(console_buffer->tail+1)%console_buffer->size;
            int cur_char=console_buffer->value[next];
            console_buffer->tail=next;
            if (cur_char==-1){
                // ctrl D then EOF, we break the loop.
                if (endfile==0){
                        endfile=1;
                }
                break;
            }
            
            main_memory[pcb->registers[6]+i+pcb->pcb_base]=cur_char;
            // printf("%d: \n", pcb->registers[6] + i + pcb->pcb_base);
            // printf("%c\n", main_memory[pcb->registers[6]+i+pcb->pcb_base]);
            V_kt_sem(nslots);
            i++;
        }
        V_kt_sem(reader);
    }else{
        P_kt_sem(pcb->fd_table[fd_index]->file_pipe->nreader);
        i = 0;
        // P_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_elem);
        // V_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_elem);
        int pipe_elem_val = kt_getval(pcb->fd_table[fd_index]->file_pipe->pipe_elem);
        int size = pcb->registers[7];
        int rblock;
        if(pipe_elem_val < pcb->registers[7]){
            rblock = 1 ;
        }else{
            rblock = 0 ;
        }
        // printf("-------read from pipe buffer------\n");
        while(i < size){
            if(i >= pipe_elem_val){
                if (pcb->fd_table[fd_index]->file_pipe->w_count == 0) {
                    // i = 0;
                    if(endfile == 0){
                        endfile = 1;
                    }
                    // printf("break read\n");
                    break;
                } else{
                    P_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_elem);
                    break;
                }
            }
            int pipe_read_end = pcb->fd_table[fd_index]->file_pipe->tail;
            P_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_elem);
            
            main_memory[pcb->registers[6]+i+pcb->pcb_base] = pcb->fd_table[fd_index]->file_pipe->pipe_buffer[pipe_read_end];
            // printf("%c",pcb->fd_table[fd_index]->file_pipe->pipe_buffer[pipe_read_end]);
            i++;
            pcb->fd_table[fd_index]->file_pipe->tail = (pcb->fd_table[fd_index]->file_pipe->tail+1)%8192;
            V_kt_sem(pcb->fd_table[fd_index]->file_pipe->pipe_slots);
        }
        V_kt_sem(pcb->fd_table[fd_index]->file_pipe->nreader);
        // printf("\n-------end------\n");
        
    }
    
    syscall_return(pcb,i);
}

void *ioctl(void *arg){
    struct PCB *pcb=(struct PCB*)arg;
    if(ValidAddress(arg) == FALSE){
        syscall_return(pcb, -EFAULT);
    }
    if (pcb->registers[5]!=1 || pcb->registers[6]!=JOS_TCGETP){
        syscall_return(pcb,-EINVAL);
    }
    // maybe bug
    
    struct JOStermios *input= (struct JOStermios*)&main_memory[pcb->registers[7]+pcb->pcb_base];
    ioctl_console_fill(input);
    syscall_return(pcb,0);
}

void *fstat(void *arg){
    struct PCB *pcb=(struct PCB*)arg;
    int buff_size=0;
    int fd_index = pcb->registers[5];
    
    if(ValidAddress(arg) == FALSE){
        syscall_return(pcb, -EFAULT);
    }

    if(fd_index >= 64 && fd_index < 0){
        syscall_return(pcb, -EBADF);
    }

    if (pcb->registers[5]==0){
        buff_size=1;
    }else if (pcb->registers[5]==1 || pcb->registers[5]==2){
        buff_size=256;
    }else{
        buff_size=8192;
    }
    struct KOSstat *input=(struct KOSstat*)&main_memory[pcb->registers[6]+pcb->pcb_base];

    stat_buf_fill(input,buff_size);
    syscall_return(pcb,0);
}


void *getpagesize(void *arg){
    struct PCB *pcb=(struct PCB*)arg;
    syscall_return(pcb,PageSize);
}

void *my_sbrk(void *arg){
    struct PCB *pcb=(struct PCB*)arg;
    if(ValidAddress(arg) == FALSE){
        syscall_return(pcb, -EFAULT);
    }

    int increm=pcb->registers[5];
    if (pcb->sbrk_ptr==NULL){
        pcb->sbrk_ptr=0;
    }
    int prev_brk=(int)pcb->sbrk_ptr;
    // handle later
    if (prev_brk+increm>User_Limit){
        syscall_return(pcb,-ENOMEM);
    }
    pcb->sbrk_ptr+=increm;
    syscall_return(pcb,prev_brk);
}

void *my_execve(void *arg){
    struct PCB *pcb=(struct PCB*)arg;

    int str_size = 0;
    while(main_memory[pcb->registers[5]+pcb->pcb_base+str_size]){
        str_size++;
    }
    // printf("first loop finished\n");
    char* filename = (char *)malloc((str_size+1)*sizeof(char));
    for(int i = 0; i < str_size; i++){
        strncpy(filename+i, main_memory+pcb->registers[5]+pcb->pcb_base+i, 1);
    }
    filename[str_size] = '\0';

    int num_str=0;
    int ptr=pcb->registers[6]+pcb->pcb_base;
    while (TRUE){
        int addr= main_memory[ptr] +
        (main_memory[ptr+1]<<8)+
        (main_memory[ptr+2]<<16)+
        (main_memory[ptr+3]<<24);

        if (addr == 0 || addr > pcb->pcb_base+User_Limit){
            break;
        }
        num_str++;
        ptr+=4;
    }
    // printf("second loop finished\n");
    char** argv = (char**)malloc((num_str+1) * sizeof(char*));
    // ptr = pcb->registers[6]+pcb->pcb_base;
    // int comp= main_memory[ptr] +
    //     (main_memory[ptr+1]<<8)+
    //     (main_memory[ptr+2]<<16)+
    //     (main_memory[ptr+3]<<24);
    
    

    // handle the case if there is only one argument
    // if (main_memory[comp] != main_memory[pcb->registers[5] + pcb->pcb_base]){
    //     int size=0;
    //     while (main_memory[pcb->registers[5]+pcb->pcb_base+size]!='\0'){
    //         size++;
    //     }
    //     char * temp= (char*)malloc((size+1)*sizeof(char));
    //     for (int j=0; j<size; ++j){
    //         strncpy(temp+j,main_memory+pcb->registers[5]+pcb->pcb_base+j, 1);
    //     }
    //     argv[0]= (char*)malloc((size+1)*sizeof(char));
    //     temp[size]='\0';
    //     memcpy(argv[0], temp, size+1);
    //     free(temp);
    //     // printf("%s\n",temp);
    // }else{
    ptr=pcb->registers[6]+pcb->pcb_base;
    for (int i=0; i<num_str; ++i){
        int size=0;
        int addr= main_memory[ptr] +
        (main_memory[ptr+1]<<8)+
        (main_memory[ptr+2]<<16)+
        (main_memory[ptr+3]<<24);

        char* c_ptr = main_memory+addr+pcb->pcb_base;
        size = strlen(c_ptr);
        
        // while (main_memory[addr+pcb->pcb_base+size]!='\0' && main_memory[addr+pcb->pcb_base+size]!=' '){
        //     size++;
        // }

        char * temp= (char*)malloc((size+1)*sizeof(char));
        for (int j=0; j<size; ++j){
            
            memcpy(temp+j,main_memory+pcb->pcb_base+addr+j,1);
        }
        temp[size]='\0';
        argv[i] = (char*)malloc((size+1)*sizeof(char));
        memcpy(argv[i],temp, size+1);
        ptr+=4;
        free(temp);
    }
    // }
    
    argv[num_str]=NULL;
    
    
    int return_value = perform_execve(pcb, filename, argv);
    pcb->registers[2]=return_value;
    dll_append(readyq,new_jval_v((void *)pcb));
    for(int i = 0; i< num_str; i++){
        free(argv[i]);    
    }
    free(argv);
    free(filename);
    kt_exit();
}

void *getpid(void *arg){
   struct PCB *pcb=(struct PCB*)arg;
   syscall_return(pcb, pcb->pid); 
}

void *my_fork(void *arg){
    struct PCB *pcb=(struct PCB*)arg;
    int mem_loc=0;
    if (!has_empty_memory(&mem_loc)){
        syscall_return(pcb,-EAGAIN);
    }
    struct PCB *newproc= (struct PCB*)malloc(sizeof(struct PCB));
    
    // initialize and set all the fields for new process.
    newproc->pid=get_new_pid();
    newproc->pcb_base=mem_loc*User_Limit;
    newproc->pcb_limit=User_Limit;
    newproc->sbrk_ptr=pcb->sbrk_ptr;
    newproc->mem_loc=mem_loc;
    memory_chunk[newproc->mem_loc]=1;
    newproc->parent=pcb;
    newproc->waiter_sem=make_kt_sem(0);
    newproc->waiters=new_dllist();
    newproc->children = make_jrb();

    jrb_insert_int(pcb->children, newproc->pid, new_jval_v((void*)newproc));

    // copy the registers.
    for (int i=0; i<NumTotalRegs; ++i){
        newproc->registers[i]=pcb->registers[i];
    }

    for(int i = 0; i < 64; i++){
        newproc->fd_table[i] = pcb->fd_table[i];
        if(pcb->fd_table[i] != NULL){
            if(pcb->fd_table[i]->console_indicator == 0 && pcb->fd_table[i]->read_or_write == 0){
                pcb->fd_table[i]->file_pipe->r_count ++;
            }else if(pcb->fd_table[i]->console_indicator == 0 && pcb->fd_table[i]->read_or_write == 1){
                pcb->fd_table[i]->file_pipe->w_count ++;
            }
        }
    }

    // memory copy the from the current process.
    memcpy(main_memory+newproc->pcb_base,main_memory+pcb->pcb_base,User_Limit);

    // fork the new process
    kt_fork(finish_fork,(void *)newproc);
    syscall_return(pcb,newproc->pid);

}


void *finish_fork(void *arg){
    struct PCB *pcb=(struct PCB*)arg;
    syscall_return(pcb,0);
}

void *my_exit(void *arg){
    struct PCB *pcb=(struct PCB*)arg;
    pcb->exit_value=pcb->registers[5];
    memory_chunk[pcb->mem_loc]=0;
    memset(main_memory+pcb->pcb_base, 0, pcb->pcb_limit);
    // for(int i = 0; i < pcb->pcb_limit; i++){
    //     if(main_memory[pcb->pcb_base + i] != 0){
    //         printf("%d: %c ",i, main_memory[pcb->pcb_base+i]);
    //     }
    // }
    // printf("\n");
    // dll_append(pcb->parent->waiters,new_jval_v((void*)pcb));
    // V_kt_sem(pcb->parent->waiter_sem);

    for(int i = 0; i < 64; i++){
        if(pcb->fd_table[i] != NULL){
            file_close(pcb, i);
        }
    }

    jrb_delete_node(jrb_find_int(pcb->parent->children, pcb->pid));

    JRB ptr;
    jrb_traverse(ptr, pcb->children){
        jrb_insert_int(sentinel->children, jval_i(ptr->key), ptr->val);
        struct PCB* temp = (struct PCB*)(ptr->val.v);
        temp->parent = sentinel;
    }
    jrb_free_tree(pcb->children);

    if(!dll_empty(pcb->waiters)){
        Dllist ptr;
        dll_traverse(ptr, pcb->waiters){
            struct PCB* child=(struct PCB*)(ptr->val.v);
            dll_delete_node(ptr);
            destroy_pid(child->pid);
            for (int i=0; i<NumTotalRegs; ++i){
                child->registers[i]=0;
            }
            free(child);
        }
    }

    
    
    // printf("here we are\n");
    if(pcb->parent == sentinel){
        destroy_pid(pcb->pid);
        for (int i=0; i<NumTotalRegs; ++i){
            pcb->registers[i]=0;
        }
        free(pcb);
    }else{
        V_kt_sem(pcb->parent->waiter_sem);
        dll_append(pcb->parent->waiters,new_jval_v((void*)pcb));
        
    }
    kt_exit();
}

void *getdtablesize(void *arg){
    struct PCB *pcb=(struct PCB*)arg;
    syscall_return(pcb,64);
}

void *my_close(void *arg){
    struct PCB *pcb=(struct PCB*)arg;
    int fd_index = pcb->registers[5];
    if(fd_index >= 64 || fd_index < 0 || pcb->fd_table[fd_index] == NULL){
        syscall_return(pcb, -EBADF);
    }
    file_close(pcb, fd_index);
    syscall_return(pcb,0);
}

void *wait(void *arg){
    struct PCB *pcb=(struct PCB*)arg;
    JRB ptr = jrb_first(pcb->children);
    if(ptr == jrb_nil(pcb->children) && dll_empty(pcb->waiters)){
        syscall_return(pcb, -1);
    }
    P_kt_sem(pcb->waiter_sem);
    Dllist toDo=dll_first(pcb->waiters);
    struct PCB* child=(struct PCB*)(toDo->val.v);
    
    int ret=child->pid;
    memcpy(&(main_memory[pcb->registers[5]+ pcb->pcb_base]),&(child->exit_value),sizeof(int));
    dll_delete_node(toDo);
    destroy_pid(child->pid);
    for (int i=0; i<NumTotalRegs; ++i){
        child->registers[i]=0;
    }
    free(child);
    syscall_return(pcb,ret);
}

void *getppid(void *arg){
    struct PCB *pcb=(struct PCB*)arg;
    syscall_return(pcb,pcb->parent->pid);
}

void *my_dup(void* arg){
    struct PCB *pcb=(struct PCB*)arg;
    int old_fd = pcb->registers[5];
    if(pcb->fd_table[old_fd] == NULL){
        syscall_return(pcb, -EBADF);
    }
    int new_fd = 0;
    while(pcb->fd_table[new_fd] != NULL && new_fd < 64){
        new_fd++;
    }
    if(new_fd == 64){
        syscall_return(pcb, -EMFILE);
    }
    pcb->fd_table[new_fd] = pcb->fd_table[old_fd];
    if(pcb->fd_table[old_fd]->file_pipe){
        if(pcb->fd_table[old_fd]->read_or_write == 0){
            pcb->fd_table[old_fd]->file_pipe->r_count++;
        }else if(pcb->fd_table[old_fd]->read_or_write == 1){
            pcb->fd_table[old_fd]->file_pipe->w_count++;
        }
    }
    

    syscall_return(pcb, new_fd);

    
}

void *my_dup2(void* arg){
    struct PCB *pcb=(struct PCB*)arg;
    int old_fd = pcb->registers[5];
    if(pcb->fd_table[old_fd] == NULL){
        syscall_return(pcb, -EBADF);
    }
    int new_fd = pcb->registers[6];

    if(old_fd == new_fd){
        syscall_return(pcb,new_fd);
    }
    
    if(new_fd >= 64 || new_fd < 0){
        syscall_return(pcb, -EBADF);
    }

    if(pcb->fd_table[new_fd] != NULL){
        file_close(pcb, new_fd);
    }

    pcb->fd_table[new_fd] = pcb->fd_table[old_fd];
    if(pcb->fd_table[old_fd]->file_pipe){
        if(pcb->fd_table[old_fd]->read_or_write == 0){
            pcb->fd_table[old_fd]->file_pipe->r_count++;
        }else if(pcb->fd_table[old_fd]->read_or_write == 1){
            pcb->fd_table[old_fd]->file_pipe->w_count++;
        }
    }
    

    syscall_return(pcb, new_fd);
}

void *my_pipe(void* arg){
    struct PCB *pcb=(struct PCB*)arg;
    if(!ValidAddress(arg)){
        syscall_return(pcb, -EFAULT);
    }
    int pipe_ptr = pcb->registers[5];
    int* pipe_desc = (int*) (&(main_memory[pipe_ptr+pcb->pcb_base]));

    struct FD* fd_read = malloc(sizeof(struct FD));
    struct FD* fd_write = malloc(sizeof(struct FD));
    struct Pipe* ptr = malloc(sizeof(struct Pipe));

    

    int read_end = 0;
    while(pcb->fd_table[read_end] != NULL && read_end < 64){
        read_end++;
    }
    if(read_end == 64){
        syscall_return(pcb, -EMFILE);
    }

    int write_end = read_end + 1;
    while(pcb->fd_table[write_end] != NULL && write_end < 64){
        write_end++;
    }
    if(write_end == 64){
        syscall_return(pcb, -EMFILE);
    }

    ptr->pipe_slots = make_kt_sem(8192);
    ptr->pipe_elem = make_kt_sem(0);
    ptr->nwriter = make_kt_sem(1);
    ptr->nreader = make_kt_sem(1);

    pcb->fd_table[read_end] = fd_read;
    pcb->fd_table[write_end] = fd_write;
    
    ptr->r_count = 1;
    ptr->w_count = 1;
    ptr->head = 0;
    ptr->tail = 0;
    memset(ptr->pipe_buffer, 0, 8192);

  
    fd_read->console_indicator = 0;
    fd_read->read_or_write = 0;
    fd_read->offset = 0;
    // fd_read->head = 0;
    // fd_read->tail = 0;
    fd_read->file_pipe = ptr;

    fd_write->console_indicator = 0;
    fd_write->read_or_write = 1;
    fd_write->offset = 0;
    // fd_write->head = 0;
    // fd_write->tail = 0;
    fd_write->file_pipe = ptr;

    

    pipe_desc[0] = read_end;
    pipe_desc[1] = write_end;

    syscall_return(pcb, 0);
}

int file_close(struct PCB* arg, int fd_index){
    if(fd_index >= 64 || fd_index < 0){
        return EBADF;
    }
    struct FD *fd = arg->fd_table[fd_index];
    if(fd->console_indicator == 1){
        // fd->console_indicator = 0;
        fd = NULL;
        return 0;
    }
    
    if(fd->read_or_write == 0 && fd->console_indicator == 0){
        fd->file_pipe->r_count --;
    }else if(fd->read_or_write == 1 && fd->console_indicator == 0){
        fd->file_pipe->w_count --;
    }
    
    // if(fd->file_pipe->r_count == 0 || fd->file_pipe->w_count == 0){
        
    // }

    if (fd->file_pipe->r_count == 0 && fd->read_or_write == 0) {
        V_kt_sem(fd->file_pipe->pipe_slots);
    }

    if (fd->file_pipe->w_count == 0 && fd->read_or_write == 1) {
        while(fd->file_pipe->r_count > 0){
            V_kt_sem(fd->file_pipe->pipe_elem);
            fd->file_pipe->r_count --;
        }
    }
    arg->fd_table[fd_index] = NULL;
    return 0;
}