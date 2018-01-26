#include<linux/in.h>  
#include<linux/inet.h>  
#include<linux/socket.h>  
#include<net/sock.h>  
  
#include<linux/init.h>  
#include<linux/module.h>  
  
int get_packet(void){
	struct socket *sock;
	int ret = 0;
	int val = 1;

	sock=(struct socket *)kmalloc(sizeof(struct socket),GFP_KERNEL);
	ret=sock_create_kern(current->nsproxy->net_ns, AF_PACKET, SOCK_RAW,htons(ETH_P_ALL)  == -1 ,&sock);  
  if(ret){  
      printk("get_packet:socket_create error!\n");  
  }  
  printk("get_packet:socket_create ok!\n");

  ret= kernel_setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char *)&val,sizeof(val));  
  if(ret){  
    printk("kernel_setsockopt error!\n");  
  }  

	char *recvbuf=NULL;  
  recvbuf=kmalloc(2048,GFP_KERNEL);  
  if(recvbuf==NULL){  
      printk("get_packet: recvbuf kmalloc error!\n");  
      return -1;  
  }  
  memset(recvbuf, 0, sizeof(recvbuf));  

	struct kvec vec;  
  struct msghdr msg;  
  memset(&vec,0,sizeof(vec));  
  memset(&msg,0,sizeof(msg));  
  vec.iov_base=recvbuf;  
  vec.iov_len=2048;  
  ret=kernel_recvmsg(sock, &msg, &vec, 1, 1024, 0);  
	recvbuf[ret] = '\0';

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
