#include <linux/unistd.h>  
#include <syscall.h>  
#include <sys/types.h>  
#include <stdio.h>  
  
struct process
{
    int pid;
    unsigned long proc_state;
		unsigned long proc_counter;
		unsigned long proc_utime;
    unsigned long stack_start;
    unsigned long stack_end;
    unsigned long code_start;
    unsigned long code_end;
    unsigned long data_start;
    unsigned long data_end;
    unsigned long brk_start;
    unsigned long brk;
		unsigned long map_count;
		unsigned long vma_address_start[256];
		unsigned long vma_address_end[256];
		unsigned long vma_maped[256];
};  

struct process a;  
  
int main()  
{  
    int i,j;  
  
    //printf("the result is:%d\n",syscall(223, &a, 1225));  
 		syscall(223, &a, 2025); 
		//printf("j = %d", j);
		printf("pid: %d\n", a.pid);
		printf("process state(-1 unrunnable, 0 runnable, >0 stopped): %d\n", a.proc_state);
		printf("process current cpu:%d\n", a.proc_counter);
		printf("process cpu time:%ld\n", a.proc_utime);
		printf("code address: %x---%x\n", a.code_start, a.code_end);  
		printf("data address: %x---%x\n", a.data_start, a.data_end);
		printf("heap address: %x---%x\n", a.brk_start, a.brk);
		printf("stack address: %x---%x\n", a.stack_start, a.stack_end);
		printf("number of VMAs: %d\n", a.map_count);
		for (i = 0; i < 256; i++){
    	printf("\tthe %dth VMA attr:start address:%x   end address:%x   mapped mem:%x\n", i, a.vma_address_start[i], a.vma_address_end[i], a.vma_maped[i]); 
		}
    return 0;  
}  
