#include<linux/in.h>  
#include<linux/inet.h>  
#include<linux/socket.h>  
#include<net/sock.h>  
#include<linux/uio.h>
  
#include<linux/init.h>  
#include<linux/module.h>  
  
int get_packet(void){
	struct socket *sock;
	int ret = 0;
	int val = 1;  

	char *recvbuf=NULL;

	struct kvec vec;
	struct msghdr msg;
	struct iovec iov;
	struct sockaddr_storage address;
	int err, err2;

	mm_segment_t fs;	

	sock=(struct socket *)kmalloc(sizeof(struct socket),GFP_KERNEL);
	ret=sock_create_kern(current->nsproxy->net_ns, PF_PACKET, SOCK_RAW,htons(ETH_P_ALL)  == -1 ,&sock);  
  if(ret){  
    printk("get_packet:socket_create error!\n");  
  }  
  printk("get_packet:socket_create ok!\n");

  ret = kernel_setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char *)&val,sizeof(val));  
  if(ret){  
    printk("get_packet: kernel_setsockopt error!\n");  
  }  
	printk("get_pakcet: kernel_setsockopt ok!\n");
  
  recvbuf=kmalloc(2048,GFP_KERNEL);  
  if(recvbuf==NULL){  
      printk("get_packet: recvbuf kmalloc error!\n");  
      return -1;  
  }  
  memset(recvbuf, 0, sizeof(recvbuf));  
	printk("get_packet: recvbuf kmalloc ok!\n");	

	//fs=get_fs();
	//set_fs(KERNEL_DS);
	//err = import_single_range(READ, recvbuf, 2048, &iov, &msg.msg_iter);
	//printk(KERN_ALERT"%d\n", err);
	//msg.msg_control = NULL;
	//msg.msg_controllen = 0;
	//msg.msg_name = /*addr ? (struct sockaddr *)&address : */NULL;
	//msg.msg_namelen = 0;
	//msg.msg_iocb = NULL;
	//err = sock_recvmsg(sock, &msg, O_NONBLOCK);
	//set_fs(fs);
  memset(&vec,0,sizeof(vec));  
  memset(&msg,0,sizeof(msg));  
  vec.iov_base=recvbuf;  
  vec.iov_len=2048;  
	printk("start get packet:\n");

	while(1){
  ret = kernel_recvmsg(sock, &msg, &vec, 1, 2048, 0);  
	recvbuf[ret] = '\0';
	printk(KERN_INFO"received bytes: %d\n", ret);
	}
	return 0;
}

static int get_packet_init(void){  
	printk("get_packet init:\n");  
	get_packet();  
	return 0;  
}         
  
static void get_packet_exit(void){  
	printk("good bye\n");  
}  
  
module_init(get_packet_init);  
module_exit(get_packet_exit);  
  
MODULE_LICENSE("GPL");    
