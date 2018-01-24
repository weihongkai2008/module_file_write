/* capture_packet.c - 截获所有以太网帧数据并进行具体分析 */
 
/* 常用函数的头文件   */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <strings.h>
#include <unistd.h> 
#include <signal.h>
 
/* 与网络相关的头文件 */
#include <netinet/ip_icmp.h>
#include <net/if_arp.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <netinet/ip.h> 
#include <netdb.h> 
#include <netinet/tcp.h> 
#include <netinet/udp.h>
#include <signal.h> 
#include <net/if.h> 
#include <sys/ioctl.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <linux/igmp.h>
#include <netinet/tcp.h>
 
 
/* 全局变量结构的结构体原型 - 包含要记录的任何全局信息 */
struct global_info {
    unsigned int bytes;     /* 网卡接收的总字节数     */
    unsigned int packet_num;        /* 网卡接受的帧的总数量   */
     
    unsigned int packet_arp;        /* 接收到的arp包的数量    */
    unsigned int packet_rarp;       /* 接收到的rarp包的数量   */
 
    unsigned int packet_ip;     /* 接收到的ip包的数量     */
    unsigned int packet_icmp;   /* 接收到的icmp包的数量   */
    unsigned int packet_igmp;   /* 接收到的igmp包的数量   */
 
    unsigned int packet_tcp;    /* 接收到的tcp包的数量    */
    unsigned int packet_udp;    /* 接收到的udp包的数量    */
     
    int print_flag_frame;       /* 是否打印帧头信息标志, 1表示打印, 0表示不打印 */
    int print_flag_arp;     /* 是否打印arp头信息标志  */
    int print_flag_ip;      /* 是否打印ip头信息标志   */
    int print_flag_rarp;        /* 是否打印rarp头信息标志 */
    int print_flag_tcp;     /* 是否打印tcp头信息标志  */
    int print_flag_udp;     /* 是否打印udp头信息标志  */
    int print_flag_icmp;        /* 是否打印icmp头信息标志 */
    int print_flag_igmp;        /* 是否打印igmp头信息标志 */
};
 
 
/* 定义一个全局变量，用于存储全局信息 */
struct global_info global;
 
struct ip_pair {
    unsigned int source_ip;
    unsigned int dest_ip;
    unsigned int count;
};
 
/* 定义一个用于存储ip对的结构体数组 */
struct ip_pair ip_pair[10000];
 
/* 一个用于初始化全局信息的函数 */
void init_global( struct global_info  * var );
 
/* 一个用于打印全局信息的函数   */
void print_global( struct global_info var );
 
/* 打印一个错误,并退出       */
void error_and_exit( char * msg, int exit_code );
 
/* 设置网卡成混杂模式            */
int set_card_promisc( char * interface_name, int sock );
 
/* 把mac地址转换一个字符串      */
void mac_to_str( char * buf, char * mac_buf );
 
/* 用于打印帮助信息         */
void help( void );
 
/* 截获网卡帧数据,并进行数据分用*/
void do_frame( int sockfd ); 
 
 
/* 处理ip层数据              */
void do_ip( char * data );
 
/* 打印ip头信息          */
void print_ip( struct iphdr * );
 
/* 处理tcp层数据            */
void do_tcp( char * data );
 
/* 打印tcp层头信息           */
void print_tcp( struct tcphdr * );
 
/* 处理udp层数据            */
void do_udp( char * data );
 
/* 打印udp层头信息           */
void print_udp( struct udphdr * );
 
 
/* 初始化一个全局结构体         */
void init_global( struct global_info  * var )
{
    var->bytes = 0;
    var->packet_num = 0;
     
    var->packet_arp = 0;
    var->packet_rarp = 0;
    var->packet_ip = 0;
    var->packet_icmp = 0;
    var->packet_igmp = 0;
    var->packet_tcp = 0;
    var->packet_udp = 0;
     
    var->print_flag_frame = 0;
    var->print_flag_arp = 0;
    var->print_flag_ip = 0;
    var->print_flag_rarp = 0;
    var->print_flag_tcp = 0;
    var->print_flag_udp = 0;
    var->print_flag_icmp = 0;
    var->print_flag_igmp = 0;
}
 
/* 一个用于打印全局信息的函数  */
void print_global( struct global_info var )
{
    printf("\n\n********** 全局信息 *****************\n\n");
    printf("总共接收字节数: %d kbytes.\n", var.bytes / 1024 );
    printf("总共接受包数量: %d\n\n", var.packet_num );
     
    if( var.packet_arp ) printf("接收 arp 包数量: %d\n", var.packet_arp );
    if( var.packet_rarp) printf("接收 rarp 包数量: %d\n", var.packet_rarp );
    if( var.packet_ip )  printf("接收 ip 包数量: %d\n", var.packet_ip );
    if( var.packet_icmp) printf("接收 icmp 包数量: %d\n", var.packet_icmp );
    if( var.packet_igmp) printf("接收 igmp 包数量: %d\n", var.packet_igmp );
    if( var.packet_tcp ) printf("接收 tcp 包数量: %d\n", var.packet_tcp );
    if( var.packet_udp ) printf("接收 udp 包数量: %d\n", var.packet_udp );
     
    printf("\n");
}
 
/* 用于处理当下按ctrl-c时的处理函数 */
void sig_int( int sig )
{
    print_global( global );
     
    int i;
     
    /*
    for( i=0; i<global.packet_ip; i++ ){
        printf("%15s ==>> ", inet_ntoa( *(struct in_addr *)( &ip_pair[i].source_ip ) ) );
        printf("%15s \n", inet_ntoa( *(struct in_addr *)( &ip_pair[i].dest_ip ) ));
    }
    */
 
    exit( 0 );
}
 
/* 打印错误信息，并退出            */
void error_and_exit( char * msg, int exit_code ) 
{ 
    herror( msg ); 
    exit( exit_code ); 
} 
 
/* 设置网卡模式成混帐模式,这样的话可以截获以太网帧数据 */
int set_card_promisc( char * interface_name, int sock ) 
{ 
    /* 用于套接口ioctl的接口请求结构体   */
    struct ifreq ifr;
                 
    /* 复制网卡名称进入请求结构体的名称元素 */
    strncpy(ifr.ifr_name, interface_name ,strlen( interface_name )+1); 
 
    /* 通过ioctl获得相应信息            */
    if((ioctl(sock, SIOCGIFFLAGS, &ifr) == -1)) {          
        error_and_exit("ioctl", 2); 
    } 
 
    /* 设置网卡模式标志为混杂模式           */
        ifr.ifr_flags |= IFF_PROMISC;                  
 
    /* 通过ioctl把参数传递给网卡         */ 
    if(ioctl(sock, SIOCSIFFLAGS, &ifr) == -1 )           
        error_and_exit("ioctl", 3); 
} 
 
/* 把mac地址转换成字符串 */
void mac_to_str( char * buf, char * mac_buf )
{
    sprintf( mac_buf, "%02x:%02x:%02x:%02x:%02x:%02x",(unsigned char) *buf, (unsigned char)(*(buf+1)),
                    (unsigned char)(*(buf+2)), (unsigned char)(*(buf+3)),
                    (unsigned char)(*(buf+4)), (unsigned char)(*(buf+5)));
    mac_buf[17] = 0;
}
 
void help( void )
{
    printf("Usage: capture [-h] [协议名称 ...].\n");
    printf("默认情况: 打印所有包信息.\n");
}
 
void print_udp( struct udphdr * pudp )
{
    printf("==================== udp header info ======================\n");
    printf("port info:  %d -> %d\n", ntohs( pudp->source ), ntohs( pudp->dest ) );
    //printf("16位目的端口号:   %d\n", ntohs( pudp->dest ) );
    //printf("16位udp长度: %d\n", ntohs( pudp->len ) );
    //printf("16位udp校验和: %d\n", ntohs( pudp->check ) );
    if( ntohs( pudp->len ) != sizeof(struct udphdr ) && ntohs( pudp->len ) < 20 ){
        char * data = ( char * )pudp + sizeof( struct udphdr );
    //    printf("UDP数据: %s\n", data );
    }
}
 
void do_udp( char * data )
{
    global.packet_udp ++;
 
    struct udphdr * pudp = ( struct udphdr * )data;
    if( global.print_flag_udp )
        print_udp( pudp ); 
}
 
 
void print_tcp( struct tcphdr * ptcp )
{
    printf("==================== tcp header info =====================\n");
    printf("port info:  %d -> %d\n", ntohs( ptcp->source ), ntohs( ptcp->dest ) );
    //printf("目的端口号: %d\n", ntohs( ptcp->dest ) );
    //printf("32位序列号  : %u\n", ntohl( ptcp->seq ) );
    //printf("32位确认序号: %u\n", ntohl( ptcp->ack_seq ) );
    //printf("首部长度: %d\n", ptcp->doff * 4 );
    //printf("6个标志位: \n");
    //printf("    紧急指针 urg : %d\n", ptcp->urg );
    //printf("    确认序号位 ack : %d\n", ptcp->ack );
    //printf("    接受方尽快将报文交给应用层 psh : %d\n", ptcp->psh );
    //printf("    重建连接 rst : %d\n", ptcp->rst );
    //printf("    用来发起连接的同步序号 syn : %d\n", ptcp->syn );
    //printf("    发送端完成任务 fin : %d\n", ptcp->fin );
    //printf("16位窗口大小: %d\n", ntohs( ptcp->window ) );
    //printf("16位校验和: %d\n", ntohs( ptcp->check ) );
    //printf("16位紧急指针: %d\n", ntohs( ptcp->urg_ptr ) );
         
    //if( ptcp->doff * 4 == 20 ){
    //    printf("选项数据: 没有\n");
    //} else {
    //    printf("选项数据: %d 字节\n", ptcp->doff * 4 - 20 );
    //}
     
    char * data = ( char * )ptcp;
    data += ptcp->doff * 4;
    //printf("数据长度: %d 字节\n", strlen(data) );
}
 
void do_tcp( char * data )
{
    global.packet_tcp ++;
 
    struct tcphdr * ptcp;
    ptcp = ( struct tcphdr * )data;
     
    if( global.print_flag_tcp )
        print_tcp( ptcp );
}
 
void print_ip( struct iphdr * iph )
{
    printf("=============== ip header info ===============\n");
    //printf("IP 首部长度:%d\n", iph->ihl * 4 );
    //printf("IP 版本    :%d\n", iph->version );
    //printf("服务类型(tos): %d\n", iph->tos );
    //printf("总长度字节: %d\n", ntohs(iph->tot_len) );
    //printf("16位标识: %d\n", ntohs(iph->id) );
    //printf("frag off: %d\n", ntohs(iph->frag_off) );
    //printf("8位生存事件: %d\n", iph->ttl );
    //printf("8位协议: %d\n", iph->protocol );
    //printf("16位首部校验和: %d\n", ntohs(iph->check) );
    printf("source IP address : %s\n", inet_ntoa( *(struct in_addr *)(&iph->saddr)) );
    printf("destination IP address : %s\n", inet_ntoa( *(struct in_addr *)(&iph->daddr)) );
         
}
 
void ip_count( struct iphdr * iph )
{
    ip_pair[ global.packet_ip - 1 ].source_ip = iph->saddr;
    ip_pair[ global.packet_ip - 1 ].dest_ip = iph->daddr;
}
 
void do_ip( char * data )
{
    global.packet_ip ++;
 
    struct iphdr *pip;        
    pip = ( struct iphdr * ) data;    /* pip = point to ip layer */
    if( global.print_flag_ip )
        print_ip( pip );
     
    ip_count( pip );
     
    char * pdata = data + pip->ihl * 4;
     
    switch( pip->protocol ){
        case IPPROTO_ICMP:
            //do_icmp( pdata );
            break;
        case IPPROTO_IGMP:
            //do_igmp( pdata );
            break;
        case IPPROTO_TCP:
            do_tcp( pdata );
            break;
        case IPPROTO_UDP:
            do_udp( pdata );
            break;
        default:
            printf("IP: 未知其上层协议.\n");
            break;
    }
}
 
/* 打印以太网帧的包头信息 */
void print_frame( struct ether_header * peth )
{
    /* 定义一个数组，用于存储把mac地址转换成字符串后的字符串 */
    char buf[ 18 ];
 
    printf("\n==================================    %d th packets  =======================================\n\n", global.packet_num );
    printf("==== ethernet info =====\n");
    char * shost = peth->ether_shost;
    mac_to_str( shost, buf );      
    printf("source ethernet address:  %s\n", buf );
     
    char * dhost = peth->ether_dhost;
    mac_to_str( dhost, buf );
    printf("destination ethernet address:  %s\n", buf );
}
 
 
/* 用于从网卡接受一帧数据,同时根据以太网协议字段传递数据给相应的上层协议处理 */
void do_frame( int sock )
{
    /* 用于存储一帧数据         */
    char frame_buf[ 2000 ];
     
    /* 清空帧数据缓冲区     */
    bzero( frame_buf, sizeof(frame_buf) );
 
    int len = sizeof( frame_buf );
     
    /* 用于存储接受字节数       */
    int recv_num;
 
    /* 用于存储发送方的地址信息 */
    struct sockaddr_in addr;
 
    /* 从网卡接收一帧数据       */
        recv_num = recvfrom( sock, (char *)frame_buf, sizeof( frame_buf ), 0, ( struct sockaddr * )&addr, &len ); 
 
    /* 所接收的包的总数自加1    */
    global.packet_num ++;
 
    /* 从网卡接收的字节总数     */
    global.bytes += recv_num;
 
    /* 打印接收的包是第几个包   */
    //printf("此帧数据长度: %d\n", recv_num );
     
    /* 定义一个用于指向以太网帧的指针 (这里我们只考虑最常见的以太网帧的情况) */
    struct ether_header * peth;
 
    /* 让以太网头指针指向从网卡接受到的帧的数据的开头 */
        peth = (struct ether_header *)frame_buf;
 
    /* 传递以太网帧首地址给打印以太网帧信息的打印函数 */
    if( global.print_flag_frame )
        print_frame( peth );
 
    /* 定义一个数据指针,用于指向以太网帧的数据部分    */
    char * pdata;
 
    /* 让 pdata 指向以太网帧的数据部分                */
    pdata = frame_buf + sizeof( struct ether_header );
 
    /* 根据以太网帧的协议字段进行数据分用 － 也就是进行数据拆封，根据协议字段调用相应层的处理函数 */
        switch( ntohs( peth->ether_type ) ){
            case ETHERTYPE_PUP:
            break;
        case ETHERTYPE_IP:
            do_ip( pdata );
            break;
        case ETHERTYPE_ARP:
            //do_arp( pdata );
            break;
        case ETHERTYPE_REVARP:
            //do_rarp( pdata );
            break;
        default:
            printf("Unkonw ethernet type  %d %x.\n", ntohs(peth->ether_type), ntohs(peth->ether_type) );
            break;
       }
}
 
 
/* 主函数, 处理命令行输入, 设置好全局变量, 并调用接受和处理帧的函数 */
 
int main( int argc, char ** argv ) 
{ 
    /* 用于存储套接口文件描述符 */
    int sockfd;
 
    /* 初始化全局变量    */
    init_global( &global );
 
    if( argc == 1 ) {               /* 表示打印所有包头信息 */
        global.print_flag_frame = 1;
        global.print_flag_arp = 1;
        global.print_flag_ip = 1;
        global.print_flag_rarp = 1;
        global.print_flag_tcp = 1;
        global.print_flag_udp = 1;
        global.print_flag_icmp = 1;
        global.print_flag_igmp = 1;
    } else {               /* 帮助 或者 通过指定协议名称只打印某层些协议 */
        if( !strcasecmp( argv[1], "-h" ) ){
            help();
            exit( 0 );
        } else {
            int i;
            for( i=1; i < argc; i++ ){
                if( !strcasecmp( argv[i], "frame" ) )
                    global.print_flag_frame = 1;
                else if( !strcasecmp( argv[i], "arp" ) )
                    global.print_flag_arp = 1;
                else if( !strcasecmp( argv[i], "rarp" ) )
                    global.print_flag_rarp = 1;
                else if( !strcasecmp( argv[i], "ip" ) )
                    global.print_flag_ip = 1;
                else if( !strcasecmp( argv[i], "tcp" ) )
                    global.print_flag_tcp = 1;
                else if( !strcasecmp( argv[i], "udp" ) )
                    global.print_flag_udp = 1;
                else if( !strcasecmp( argv[i], "icmp" ) )
                    global.print_flag_icmp = 1;
                else if( !strcasecmp( argv[i], "igmp" ) )
                    global.print_flag_igmp = 1;
            }
        }
    }
     
    /* 通过协议族AF_PACKET类信SOCK_RAW, 类型SOCK_RAW创建一个用于可以接受网卡帧数据的套接口,同时返回套就口文件描述符 */
    if( (sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) ) == -1 )
            error_and_exit( "socket", 1 );  /* 如果发生错误，返回错误值, 并退出 */
 
     
    /* 设定网卡eth0成混杂模式 */
    //set_card_promisc( "eth0", sockfd );
 
    /* 设定信号处理函数, 下面是设置当我们按下ctrl-c时所调用的处理函数 */  
    signal( SIGINT, sig_int );
 
    /* 无限循环接收以太网卡数据帧, 并进行数据分用,直到你按下ctrl-c */
    while( 1 ){
        do_frame( sockfd );
    } 
 
    return 0;
}
