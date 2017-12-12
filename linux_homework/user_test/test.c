#include <linux/unistd.h>  
#include <syscall.h>  
#include <sys/types.h>  
#include <stdio.h>  
  
struct process
{
    int pid;
    int depth;
    unsigned long stack_start;
    unsigned long stack_end;
    unsigned long code_start;
    unsigned long code_end;
    unsigned long data_start;
    unsigned long data_end;
    unsigned long brk_start;
    unsigned long brk;
};  

struct process a;  
  
int main()  
{  
    int i,j;  
  
    printf("the result is:%d\n",syscall(223, &a, 1225));  
  
    printf("%d\n",a.pid);  
    printf("stack: %x--%x\n",a.stack_start, a.stack_end);
  
    return 0;  
}  
