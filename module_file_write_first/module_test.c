#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/timekeeping.h>

#define FILE_BLOCK 2048 

void do_gettimeofday(struct timeval *tv){
	struct timespec now;

	getnstimeofday(&now);
	tv->tv_sec = now.tv_sec;
	tv->tv_usec = now.tv_nsec/1000;
}

static long delta(struct timeval *tv1,struct timeval *tv2){
	unsigned long deltv;

	deltv = tv2->tv_sec - tv1->tv_sec;
	deltv = deltv*1000000 + tv2->tv_usec - tv1->tv_usec;

	return deltv;
}

void fileread(const char * filename)
{
  struct file        *filp; // structure file represent a file that be opened
  struct inode     *inode;  //in the kernel, structure inode represent specific file
							//structure file represent the file desciption that have been opened
  mm_segment_t       fs;

  off_t   fsize;
  char    *buf;
  unsigned long  magic;
  int file_once_block;
	printk("<1>start....\n");
  filp=filp_open(filename,O_RDONLY,0);          //file open in kernel
  inode=filp->f_inode;                          //the inode infomation of specific file

  magic=inode->i_sb->s_magic;

  printk("<2>file system magic:%li \n",magic);
  printk("<3>super blocksize:%li \n",inode->i_sb->s_blocksize);
  printk("<4>inode %li \n",inode->i_ino);
  fsize=inode->i_size;
  printk("<5>file size:%i \n",(int)fsize);

  buf=(char *) kmalloc(FILE_BLOCK,GFP_ATOMIC);//get the mem from slab with the limit of maximum
  
  filp->f_op->llseek(filp,0,0);
  fs=get_fs();
  set_fs(KERNEL_DS);
  //filp->f_op->read(filp,buf,fsize,&(filp->f_pos));  //error....why?
  while(file_once_block = vfs_read(filp, buf, FILE_BLOCK, &(filp->f_pos))){
		//printk("file once read:%d\n", file_once_block);
  }
	set_fs(fs);

  //buf[fsize]='\0';
  printk("<6>The File Content is:\n");
  printk("<7>%s",buf);
  filp_close(filp,NULL);
}

void filewrite(char* filename, char* data)
{
  struct file *filp_out, *filp_in;
	char *buf;
	long ftime;
	long count_time = 0;
	struct timeval tva, tvb;	

  mm_segment_t fs;
  filp_out = filp_open("/home/weihongkai/Desktop/test_out.bin", O_RDWR|O_TRUNC|O_CREAT, 0777);
	filp_in=filp_open("/home/weihongkai/Desktop/test.bin",O_RDONLY,0); 
	
	printk("log\n");
	buf=(char *) kmalloc(FILE_BLOCK,GFP_ATOMIC);
	printk("buf address: %x\n", buf);
  fs=get_fs();
  set_fs(get_ds());
	while (ftime = vfs_read(filp_in, buf, FILE_BLOCK, &(filp_in->f_pos))){
		do_gettimeofday(&tva);
  	vfs_write(filp_out, buf, ftime, &filp_out->f_pos);
  	do_gettimeofday(&tvb);
		count_time += delta(&tva, &tvb);
	}
	set_fs(fs);
	printk(KERN_INFO"kernel write cost: %lu microseconds\n",count_time);
  filp_close(filp_in, NULL);
	filp_close(filp_out, NULL);
}

static int __init module_test_init(void)
{
	long count_time = 0;
  char *filename="/home/weihongkai/Desktop/test.bin";
	struct timeval tva, tvb;

  printk("<1>Read File from Kernel.\n");
  printk("%s", filename);
  //do_gettimeofday(&tvb);
	//fileread(filename);
	//do_gettimeofday(&tvb);
	//count_time += delta(&tva, &tvb);
	//printk(KERN_INFO"kernel read cost: %lu microseconds\n",count_time);
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
