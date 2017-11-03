#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

static int count = 10;
static char *init_mesg = "hello,world\n";
static char *exit_mesg = "goodbye\n";
static int major = 252;
static int minor = 0;
dev_t devnum;
int static hello_dev_open(struct inode *inode, struct file *file)
{
         printk("file open in hello_dev_open......finished!\n");

         return 0;
}


int static hello_dev_release(struct inode *inode, struct file *file)
{
         printk("file release in hello_dev_release......finished!\n");
         return 0;
}



ssize_t hello_dev_read(struct file *file, char __user *buf,size_t count, loff_t *offset)
{
  char alpha[27];
  int i, cnt;
  memset(alpha, 0, 27);
  for(i = 0; i < 26; i++)
         alpha[i] = 'a' + i;

  if(count > 26)
         cnt = 26;
  else
         cnt = count;
//使用copy_to_user ()函数从driver读数据到user
  if(!copy_to_user((char *)buf, alpha, cnt))
         return cnt;
  else
     return -1;
}


ssize_t hello_dev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
  char alpha[27];
  int cnt;
  memset(alpha, 0, 27);
  if(count > 26)
         cnt = 26;
  else
       cnt = count;
//使用copy_from_user()函数从user写数据到driver
  if(!copy_from_user((char *)alpha, buf, cnt))
  {
         printk(alpha);
         printk("\n");
         return cnt;
  }
  else
         return -1;
}


static struct cdev hello_dev;
static struct file_operations fops ={
         .owner = THIS_MODULE,
         .open  = hello_dev_open,
         .release = hello_dev_release,
         .read = hello_dev_read,
         .write = hello_dev_write,
};

static int __init hello_init(void)
{
  int i;
  int ret;
  for(i = 0; i < count; i++)
         printk(init_mesg);
//ret = register_chrdev_region(MKDEV(major,minor), 1, "hello_dev");
  ret = alloc_chrdev_region(&devnum, 10, 1, "hello_dev");//get the dev no to operator the respond to dev
  if(!ret && ret > 0)
  {
       major = MAJOR(devnum);
         minor = MINOR(devnum);
         printk("major = %d; minor = %d\n", major, minor);
  }

  cdev_init(&hello_dev, &fops);
  hello_dev.owner = THIS_MODULE;
  hello_dev.ops = &fops;
  ret = cdev_add(&hello_dev, devnum, 1);
  printk("cdev_add return:%d\n", ret);

  return ret;
}

void hello_exit(void)
{
  printk(exit_mesg);
  cdev_del(&hello_dev);
  unregister_chrdev_region(MKDEV(major, minor),1);

  return;
}

void hello(void)
{
         printk("good mornig1\n");
}

module_param(count, int, S_IRUGO);
module_param(init_mesg, charp, S_IRUGO);
module_param(exit_mesg, charp, S_IRUGO);

EXPORT_SYMBOL_GPL(hello);
module_init(hello_init);
module_exit(hello_exit);
