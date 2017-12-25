#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <linux/in.h>

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

int main(int argc, char *argv[]){
     int  SOCKET_SRC;
     char buf[BUFFER_MAX];
     int n_rd;

     if( (SOCKET_SRC = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP))) < 0 ){
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
		
			 	ip_header *ipheader;
       	ipheader = (struct ip_header *)&buf[14];
				printf("\n-------------\nrecv %d bytes \n",n_rd);
       	printf("sourceIP:%d.%d.%d.%d\n",ipheader->saddr.byte1,ipheader->saddr.byte2,ipheader->saddr.byte3,ipheader->saddr.byte4);
       	printf("destinationIP:%d.%d.%d.%d\n",ipheader->daddr.byte1,ipheader->daddr.byte2,ipheader->daddr.byte3,ipheader->daddr.byte4);
        
			 	if(6 == (int)ipheader->proto){
            printf("TCP Packet\n");
       	}
       	else if(17 == (int)ipheader->proto){
            printf("UDP Packet\n");
       	}
				if ((ipheader->saddr.byte1 == 220) && (ipheader->saddr.byte2 == 181) && (ipheader->saddr.byte3 == 111) && (ipheader->saddr.byte4 == 188)){
				printf("package from baidu!!");
				}
     }
     close(SOCKET_SRC);
     return 0;
}

