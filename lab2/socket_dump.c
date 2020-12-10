#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <linux/if_ether.h> 
#include <netinet/in.h>
#define BUFFER_MAX 2048
int main(int argc,char* argv[]){
	int fd;
	int proto;
	int str_len;
	char buffer[BUFFER_MAX]; 
	char *ethernet_head; 
	char *ip_head;
	unsigned char *p;
	unsigned char *type;
	// 创建socketfd用来接收以太帧
	if((fd=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL)))<0) {
		printf("error create raw socket\n");
		return -1; 
	}

	while(1){
		str_len = recvfrom(fd,buffer,2048,0,NULL,NULL);
		// 想一想，此处为什么要丢弃小于 42 字节的包?
		if(str_len < 42) {
			printf("error when recv msg \n");
		return -1; 
		}
		ethernet_head = buffer;
		p = ethernet_head;
		ip_head = ethernet_head + 14;
		printf("Dst MAC address: %.2x:%02x:%02x:%02x:%02x:%02x\n",
		                        p[6],p[7],p[8],p[9],p[10],p[11]);
		printf("Src MAC address: %.2x:%02x:%02x:%02x:%02x:%02x\n", 
								p[0],p[1],p[2],p[3],p[4],p[5]);
		type = ethernet_head + 12;

		printf("类型：");
		// IEEE 802.3
		if(type[0]==0||type[0]==1||type[0]==2||type[0]==3||type[0]==4||type[0]==5){
			printf("IEEE 802.3\n"); 
		}
		// IP协议
		else if (type[1]==0)
		{
			printf("IP协议\n");
			p = ip_head + 2;
			printf("IP报文总长度：%02x%02x\n", p[0],p[1]);
			proto = (ip_head + 9)[0];
			p = ip_head + 12;
			printf("Src IP: %d.%d.%d.%d\n",p[0],p[1],p[2],p[3]);
			printf("Dst IP: %d.%d.%d.%d\n",p[4],p[5],p[6],p[7]);
			printf("Protocol:" );
			switch(proto){
				case 1:printf("icmp\n");break;
				case 2:printf("igmp\n");break;
				case 6:printf("tcp\n");break;
				case 17:printf("udp\n");break;
				case 89:printf("ospf\n");break;
				default:
				break;
			}
		}
		// ARP协议
		else if (type[1]==6)
		{
			printf("ARP协议\n");
			p = ethernet_head+14;
		    printf("Hardware type %02x%02x\n",p[0],p[1]);
            printf("Protocol type: %02x%02x\n",p[2],p[3]);
            printf("Hardware size: %d\n",p[4]);
            printf("Protocol size: %d\n",p[5]);
            printf("Opcode: %02x\n",p[7]);
            p = p+8;
            //源MAC
            printf("Sender MAC Adress: %.2x:%02x:%02x:%02x:%02x:%02x\n",p[0],p[1],p[2],p[3],p[4],p[5]);
            //源IP
            p = p+6;
            printf("Sender IP Adress: %d.%d.%d.%d\n",p[0],p[1],p[2],p[3]);
            //目的MAC
            p = p+4;
            printf("Target MAC Adress:  %.2x:%02x:%02x:%02x:%02x:%02x\n",p[0],p[1],p[2],p[3],p[4],p[5]);
            //目的IP
            p= p+6;
            printf("Target IP Adress: %d.%d.%d.%d\n",p[0],p[1],p[2],p[3]);
        }
		//RARP
		else if (type[1]==53)
		{
			printf("RARP协议\n");
		}
		// IPv6 dd
		else if (type[1]==221){
			printf("IPv6\n");
		}
		printf("=============================================================\n");
	}
	return -1; 
}