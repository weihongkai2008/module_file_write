#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>
#include <linux/sched.h>

#define my_syscall_num 223

//sudo cat /proc/kallsyms | grep sys_call_table
#define sys_call_table_address 0xffffffff8b200240

static int counter = 0;
struct process
{
	int pid;
	int depth;
	unsigned long stack_start;
	unsigned long stack_end;
};

struct process a[512];

unsigned int clear_and_return_cr0(void);
void setback_cr0(unsigned int val);
asmlinkage long sys_mycall(char __user *buf);
int orig_cr0;
unsigned long *sys_call_table = 0;
static int (*anything_saved)(void);


void processtree(struct task_struct * p,int b)
{
	struct list_head * l;
	struct mm_struct *tmp;
	//printk("log");
	//tmp = p->active_mm;
	//printk("%x\n",tmp);
	a[counter].pid = p -> pid;
	a[counter].depth = b;
	//if (!tmp){
	//	a[counter].stack_start = 0;
	//	a[counter].stack_end = 0;
	//}
	//a[counter].stack_start = tmp->start_stack;
	//a[counter].stack_end = tmp->arg_start;
	//printk("%x\n", a[counter].stack_start);
	counter ++;
	for(l = p -> children.next; l != &(p->children); l = l->next)
	{
		struct task_struct *t = list_entry(l,struct task_struct,sibling);
		processtree(t,b+1);
	}
}

unsigned int clear_and_return_cr0(void)
{
	unsigned int cr0 = 0;
	unsigned int ret;
	asm("movq %%cr0, %%rax":"=a"(cr0));
	ret = cr0;
	cr0 &= 0xfffeffff;
	asm("movq %%rax, %%cr0"::"a"(cr0));
	return ret;
}

void setback_cr0(unsigned int val)//读取val的值到eax寄存器，再将eax寄存器的值放入cr0中
{
	asm volatile("movq %%rax, %%cr0"::"a"(val));
}

static int __init init_addsyscall(void)
{
	printk("hello,lihuan kernel\n");
	sys_call_table = (unsigned long *)sys_call_table_address;//获取系统调用服务首地址
	printk("%x\n",sys_call_table);
	anything_saved = (int(*)(void)) (sys_call_table[my_syscall_num]);//保存原始系统调用的地址
	orig_cr0 = clear_and_return_cr0();//设置cr0可更改
	sys_call_table[my_syscall_num]= (unsigned long)&sys_mycall;//更改原始的系统调用服务地址
	setback_cr0(orig_cr0);//设置为原始的只读cr0
	return 0;
}

asmlinkage long sys_mycall(char __user * buf)
{
  int b = 0;
	struct task_struct * p;
	struct mm_struct *tmp;
	printk("This is lihuan_syscall!\n");

	//for(p = current; p != &init_task; p = p->parent );
	//	processtree(p,b);
		
	p = current;
	tmp = p->active_mm;
	if(tmp){
	printk("pid: %d\n", p->pid);
	printk("code address: %x---%x\n", tmp->start_code, tmp->end_code);	
	printk("data address: %x---%x\n", tmp->start_data, tmp->end_data);
	printk("heap address: %x---%x\n", tmp->start_brk, tmp->brk);
	printk("stack address: %x---%x\n", tmp->start_stack, tmp->arg_start);
	printf("number of VMAs: %d\n", tmp->map_count);
	}
	//if(copy_to_user((struct process *)buf,a,512*sizeof(struct process)))
	//	return -EFAULT;
	//else
	//	return sizeof(a);
}

static void __exit exit_addsyscall(void)
{
	//设置cr0中对sys_call_table的更改权限。
	orig_cr0 = clear_and_return_cr0();//设置cr0可更改
	//恢复原有的中断向量表中的函数指针的值。
	sys_call_table[my_syscall_num]= (unsigned long)anything_saved;
	//恢复原有的cr0的值
	setback_cr0(orig_cr0);
	printk("call lihuan exit \n");
}

module_init(init_addsyscall);
module_exit(exit_addsyscall);
MODULE_LICENSE("GPL");
