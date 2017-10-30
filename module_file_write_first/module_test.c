#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#include <linux/slab.h>

void fileread(const char * filename)
{
  struct file        *filp; //  文件结构体代表一个打开的文件
  struct inode     *inode; //内核中用inode结构表示具体的文件，而用file结构表示打开的文件描述符
  mm_segment_t       fs;

  off_t   fsize;
  char    *buf;
  unsigned long  magic;
  printk("<1>start....\n");
  filp=filp_open(filename,O_RDONLY,0);          //文件结构
  inode=filp->f_inode;                          //和具体文件联系一起

  magic=inode->i_sb->s_magic;

  printk("<2>file system magic:%li \n",magic);
  printk("<3>super blocksize:%li \n",inode->i_sb->s_blocksize);
  printk("<4>inode %li \n",inode->i_ino);
  fsize=inode->i_size;
  printk("<5>file size:%i \n",(int)fsize);

  buf=(char *) kmalloc(fsize+1,GFP_ATOMIC);//get the mem from slab with the limit of maximum
  
  filp->f_op->llseek(filp,0,0);
  fs=get_fs();
  set_fs(KERNEL_DS);
  //filp->f_op->read(filp,buf,fsize,&(filp->f_pos));  //error....why?
  vfs_read(filp, buf, fsize, &(filp->f_pos)); 
  set_fs(fs);

  buf[fsize]='\0';
  printk("<6>The File Content is:\n");
  printk("<7>%s",buf);
  filp_close(filp,NULL);
}

void filewrite(char* filename, char* data)
{
  struct file *filp;
  mm_segment_t fs;
  filp = filp_open(filename, O_RDWR|O_APPEND, 0644);
  if(IS_ERR(filp))

  fs=get_fs();
  set_fs(get_ds());
  vfs_write(filp, data, strlen(data),&filp->f_pos);
  set_fs(fs);
  filp_close(filp,NULL);
}

static int __init module_test_init(void)
{
  char *filename="/home/weihongkai/Desktop/helloworld.c";

  printk("<1>Read File from Kernel.\n");
  printk("%s", filename);
  fileread(filename);
  filewrite(filename, "kernel write test\n");
  return 0;
}

static void __exit module_test_exit(void)
{
  printk("<1>Good,Bye!\n");
}
module_init(module_test_init);
module_exit(module_test_exit);
MODULE_LICENSE("GPL");
