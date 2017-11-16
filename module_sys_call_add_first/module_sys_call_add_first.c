#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>
#include <linux/sched.h>

#define my_syscall_num 223
//sudo grep sys_call_table /boot/System.map-`uname -r`
#define sys_call_table_adress 0xffffffff81a00240


unsigned int clear_and_return_cr0(void);
void setback_cr0(unsigned int val);
asmlinkage long sys_mycall(void);

int orig_cr0;
unsigned long *sys_call_table = 0;
static int (*anything_saved)(void);

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

void setback_cr0(unsigned int val) //读取val的值到eax寄存器，再将eax寄存器的值放入cr0中
{
 asm volatile("movq %%rax, %%cr0"::"a"(val));
}

static int __init init_addsyscall(void)
{
 printk("hello, kernel\n");
 sys_call_table = (unsigned long *)sys_call_table_adress;//获取系统调用服务首地址
 anything_saved = (int(*)(void)) (sys_call_table[my_syscall_num]);//保存原始系统调用的地址
 orig_cr0 = clear_and_return_cr0();//设置cr0可更改
 sys_call_table[my_syscall_num] = (unsigned long)&sys_mycall;//更改原始的系统调用服务地址
 setback_cr0(orig_cr0);//设置为原始的只读cr0
 return 0;
}

asmlinkage long sys_mycall(void)
{
 printk("This is my_syscall!\n");
 return current->pid;
}

static void __exit exit_addsyscall(void)
{
 //设置cr0中对sys_call_table的更改权限。
 orig_cr0 = clear_and_return_cr0();//设置cr0可更改

 //恢复原有的中断向量表中的函数指针的值。
 sys_call_table[my_syscall_num] = (unsigned long)anything_saved;
 
 //恢复原有的cr0的值
 setback_cr0(orig_cr0);

 printk("call exit \n");
}

module_init(init_addsyscall);
module_exit(exit_addsyscall);
MODULE_LICENSE("GPL");
