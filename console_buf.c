#include "console_buf.h"

void *read_from_console(void *arg){
    int done=0;
    while (!done){
        P_kt_sem(consoleWait);      
        
        char cur=console_read();
        P_kt_sem(nslots);
        if (cur==-1){
            // if EOF or ctrl-D, break the loop
            done=1;
        }
        V_kt_sem(nelem);
        int next=(console_buffer->head+1)%console_buffer->size;
        console_buffer->value[next]=cur;
        console_buffer->head=next;
        
    }
    // for (int i=0; i<BUFFERSIZE;++i){
    //     printf("%c",console_buffer->value[i]);
    // }
    // printf("\n");
    kt_exit();
}