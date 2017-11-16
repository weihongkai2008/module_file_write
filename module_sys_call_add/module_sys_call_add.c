#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>
#include <linux/sched.h>

#define my_syscall_num 223
#define sys_call_table_address 0xffffffffa9c00240

//sudo cat /proc/kallsyms | grep sys_call_table

unsigned long long *sys_call_table = 0;
static unsigned long long*orig_saved;

int make_rw(unsigned long long address)  
{  
        unsigned int level;  
        pte_t *pte = lookup_address(address, &level);//查找虚拟地址所在的页表地址  
        if (pte->pte & ~_PAGE_RW)  //设置页表读写属性
                pte->pte |=  _PAGE_RW;  
          
        return 0;  
}  
  
  
  
int make_ro(unsigned long long address)  
{  
        unsigned int level;  
        pte_t *pte = lookup_address(address, &level);  
        pte->pte &= ~_PAGE_RW;  //设置只读属性
  
        return 0;  
}

asmlinkage long sys_mycall(void)
{
    printk(KERN_ALERT "i am hack syscall!\n");
    return 0;
}

static int syscall_init_module(void)  
{  
				sys_call_table = (unsigned long long*)sys_call_table_address;
        printk(KERN_ALERT "sys_call_table: 0x%p\n", sys_call_table_address);
        orig_saved = (unsigned long long*)(sys_call_table[223]);  //保存原有的223号的系统调用表的地址
        printk(KERN_ALERT "orig_saved : 0x%p\n", orig_saved );  
  
        make_rw((unsigned long long)sys_call_table);  //修改页的写属性
        sys_call_table[223] = (unsigned long*)sys_mycall;  //将223号指向自己写的调用函数
        make_ro((unsigned long long)sys_call_table);  
  
        return 0;  
}

static void syscall_cleanup_module(void)  
{  
        printk(KERN_ALERT "Module syscall unloaded.\n");  
  
        make_rw((unsigned long long)sys_call_table);  
        sys_call_table[223] = (unsigned long long*) orig_saved ;   
        make_ro((unsigned long long)sys_call_table);  
}

module_init(syscall_init_module);  
module_exit(syscall_cleanup_module);  
  
MODULE_LICENSE("GPL");  
MODULE_DESCRIPTION("mysyscall");  
