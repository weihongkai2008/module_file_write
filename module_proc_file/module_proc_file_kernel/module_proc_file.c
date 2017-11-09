#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/init.h>  
#include <linux/proc_fs.h>  
#include <linux/jiffies.h>  
#include <asm/uaccess.h>  
#include <linux/seq_file.h>
  
  
#define MODULE_VERS "1.0"  
#define MODULE_NAME "module_proc_file"  
  
#define FOOBAR_LEN 8  

char msg[512] = {0};
  
struct fb_data_t {  
	    char name[FOOBAR_LEN + 1];  
		char value[FOOBAR_LEN + 1];  
};  
  
  
static struct proc_dir_entry *example_dir, *foo_file;    
  
struct fb_data_t foo_data;  

static ssize_t module_proc_file_write(struct file *file, const char *data,size_t len,loff_t *off)
{

	if(copy_from_user(msg,(void*)data,len))
		return -EFAULT;
	msg[len]='\0';
	return len;
}

static ssize_t module_proc_file_read(struct file *filp, char __user *data, size_t len, loff_t *off)
{
	
	if(*off > 0)
		return 0;
	if(copy_to_user(data,msg,strlen(msg)))
		return -EFAULT;
	*off += strlen(msg);
	printk("content:%s\n", msg);
	return strlen(msg);
}


  
static struct file_operations proc_seq_fops = 
{
		.owner		= THIS_MODULE,
		.read = module_proc_file_read,
		.write = module_proc_file_write
};

static int __init init_procfs_example(void)  
{  
	    int rv = 0;  
		/* create directory */  
		example_dir = proc_mkdir(MODULE_NAME, NULL);  
		
		strcpy(foo_data.name, "foo");  
		strcpy(foo_data.value, "foo"); 
		
		if(example_dir == NULL) {  
			rv = -ENOMEM;  
			goto out;  
	    }  
		foo_file = proc_create_data("foo", 0777, example_dir, &proc_seq_fops, &foo_data);  
		if(foo_file == NULL) {  
			rv = -ENOMEM;  
			goto no_foo;  
	    }  
						  
		/* everything OK */  
		printk(KERN_INFO "%s %s initialised\n",  
		MODULE_NAME, MODULE_VERS);  
		return 0;  
													  
													 
no_foo:  
		remove_proc_entry("foo", example_dir);  
														 
out:  
		return rv;  
}  
  
  
static void __exit cleanup_procfs_example(void)  
{  
	  
	    remove_proc_entry("foo", example_dir);    
		remove_proc_entry(MODULE_NAME, NULL);  
			  
		printk(KERN_INFO "%s %s removed\n",  
		MODULE_NAME, MODULE_VERS);  
}  
  
  
module_init(init_procfs_example);  
module_exit(cleanup_procfs_example);  
  
MODULE_AUTHOR("WeiHongkai");  
MODULE_DESCRIPTION("module proc read write");  
MODULE_LICENSE("GPL");  
