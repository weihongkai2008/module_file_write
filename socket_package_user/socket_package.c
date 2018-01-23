#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_ether.h>

#include <net/ethernet.h>
#include <netinet/tcp.h> 
#include <netinet/udp.h> 
#include <netinet/ip.h> 

#define BUFFER_MAX 2048
 
typedef struct ip_address{
    u_char byte1;
    u_char byte2;
    u_char byte3;
    u_char byte4;
}ip_address;

typedef struct ip_header{
    u_char  ver_ihl;        // Version (4 bits) + Internet header length (4 bits)
    u_char  tos;            // Type of service 
    u_short tlen;           // Total length 
    u_short identification; // Identification
    u_short flags_fo;       // Flags (3 bits) + Fragment offset (13 bits)
    u_char  ttl;            // Time to live
    u_char  proto;          // Protocol
    u_short crc;            // Header checksum
    ip_address  saddr;      // Source address
    ip_address  daddr;      // Destination address
//  u_int   op_pad;         // Option + Padding
}ip_header;

typedef struct _udp_hdr
{
	unsigned short src_port; //远端口号
	unsigned short dst_port; //目的端口号
	unsigned short uhl;   //udp头部长度
	unsigned short chk_sum; //16位udp检验和
}udp_hdr;

//typedef struct _tcp_hdr
//{
//	unsigned short src_port;   //源端口号
//	unsigned short dst_port;   //目的端口号
//	unsigned int seq_no;    //序列号
//	unsigned int ack_no;    //确认号
//}tcp_hdr;

int main(int argc, char *argv[]){
     int  SOCKET_SRC;
     char buf[BUFFER_MAX];
     int n_rd;
		 struct udphdr *udpheader;
		 struct tcphdr *tcpheader;

     if( (SOCKET_SRC = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP))) < 0 ){
         printf("create socket error.\n");
         exit(0);
     }
     while(1){
     		n_rd = recvfrom(SOCKET_SRC, buf, BUFFER_MAX, 0, NULL, NULL);
     	 	if (n_rd<46) {
         	 perror("recvfrom():");
          	printf("Incomplete packet (errno is 101)\n");
          	close(SOCKET_SRC);
          	exit(0);
    	 	}
    	 /* An Ethernet frame was written to buf, frame analysis can be processed here */
    	 /* Termination control */
		
			 	struct ip_header *ipheader;
				struct iphdr *iphdr_self;
				char *data1;
        char *data2;
       	data1 = buf + sizeof( struct ether_header );
				data2 = buf + sizeof(struct ether_header);
				ipheader = (struct ip_header *)data1;
				
			 	if(6 == (int)ipheader->proto){
            printf("TCP Packet\n");
            printf("\n-------------\nrecv %d bytes \n",n_rd);
            printf("sourceIP:%d.%d.%d.%d\n",ipheader->saddr.byte1,ipheader->saddr.byte2,ipheader->saddr.byte3,ipheader->saddr.byte4);
            printf("destinationIP:%d.%d.%d.%d\n",ipheader->daddr.byte1,ipheader->daddr.byte2,ipheader->daddr.byte3,ipheader->daddr.byte4);
						iphdr_self = (struct iphdr *)data2;	
						char *pdata = data2 + iphdr_self->ihl * 4;
						tcpheader = (struct tcphdr *)pdata;
						printf("tcp port: %d -> %d\n", ntohs(tcpheader->source), tcpheader->dest);
				}
       	else if(17 == (int)ipheader->proto){
						printf("UDP Packet\n");
            printf("\n-------------\nrecv %d bytes \n",n_rd);
            printf("sourceIP:%d.%d.%d.%d\n",ipheader->saddr.byte1,ipheader->saddr.byte2,ipheader->saddr.byte3,ipheader->saddr.byte4);
            printf("destinationIP:%d.%d.%d.%d\n",ipheader->daddr.byte1,ipheader->daddr.byte2,ipheader->daddr.byte3,ipheader->daddr.byte4);

						iphdr_self = (struct iphdr *)data2;
            char *pdata = data2 + iphdr_self->ihl * 4;
						udpheader = (struct udphdr *)pdata;
						printf("udp port: %d -> %d\n", ntohs(udpheader->source), udpheader->dest);
				}
     }
     close(SOCKET_SRC);
     return 0;
}

