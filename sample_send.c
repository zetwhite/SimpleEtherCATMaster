#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h>	//For standard things
#include<stdlib.h>	//malloc
#include<string.h>	//strlen

#include<netinet/ip_icmp.h>	//Provides declarations for icmp header
#include<netinet/udp.h>	//Provides declarations for udp header
#include<netinet/tcp.h>	//Provides declarations for tcp header
#include<netinet/ip.h>	//Provides declarations for ip header
#include<netinet/if_ether.h>	//For ETH_P_ALL
#include<net/ethernet.h>	//For ether_header 
#include <linux/if_packet.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>
#include <sys/ioctl.h> 
#include <net/if.h> 
//#include <libexplain/ioctl.h>


unsigned short checksum(unsigned short* buff, int _16bitword)
{
    unsigned long sum;
    for(sum=0;_16bitword>0;_16bitword--)
    sum+=htons(*(buff)++);
    sum = ((sum >> 16) + (sum & 0xFFFF));
    sum += (sum>>16);
    return (unsigned short)(~sum);
}

int main(){
    int sock_raw = socket(AF_PACKET,SOCK_RAW,IPPROTO_RAW);
    if(sock_raw == -1)
        printf("error on socoket\n");

    struct ifreq ifreq_i;
    memset(&ifreq_i,0,sizeof(ifreq_i));
    char* interface = "wlp0s20f3";
    int interface_len =  strlen(interface); 
    printf("%d\n", interface_len); 
    strncpy(ifreq_i.ifr_name,interface,interface_len); //giving name of Interface
    
    if((ioctl(sock_raw,SIOCGIFINDEX,&ifreq_i))<0){
        printf("error in index ioctl reading\n");//getting Index Name
        int err = errno;
        printf("error : %s\n", strerror(errno)); 
        //fprintf(stderr, "%s\n", explain_errno_ioctl(err, sock_raw, SIOCGIFINDEX, &ifreq_i));
        exit(EXIT_FAILURE);
        //return -1; 
    }

    printf("index=%d\n",ifreq_i.ifr_ifindex);
    
    struct ifreq ifreq_c;
    memset(&ifreq_c,0,sizeof(ifreq_c));
    strncpy(ifreq_c.ifr_name,interface,interface_len);//giving name of Interface
    
    if((ioctl(sock_raw,SIOCGIFHWADDR,&ifreq_c))<0) //getting MAC Address
        printf("error in SIOCGIFHWADDR ioctl reading\n");

    struct ifreq ifreq_ip;
    memset(&ifreq_ip,0,sizeof(ifreq_ip));
    strncpy(ifreq_ip.ifr_name,interface,interface_len);//giving name of Interface
    if(ioctl(sock_raw,SIOCGIFADDR,&ifreq_ip)<0) //getting IP Address
    {
        printf("error in SIOCGIFADDR \n");
    }

    unsigned char* sendbuff=(unsigned char*)malloc(64); // increase in case of more data
    memset(sendbuff,0,64);    

    struct ethhdr *eth = (struct ethhdr *)(sendbuff);
    
    eth->h_source[0] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[0]);
    eth->h_source[1] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[1]);
    eth->h_source[2] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[2]);
    eth->h_source[3] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[3]);
    eth->h_source[4] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[4]);
    eth->h_source[5] = (unsigned char)(ifreq_c.ifr_hwaddr.sa_data[5]);
    
    /* filling destination mac. DESTMAC0 to DESTMAC5 are macro having octets of mac address. */
    unsigned char destmac[6] = {0x3c, 0xf0, 0x11, 0x28, 0x2a, 0x4e}; 
    for(int i = 0; i < 6; i++){
        eth->h_dest[i] = destmac[i]; 
    }
    /*
    eth->h_dest[0] = destmac[0]; 
    eth->h_dest[1] = DESTMAC1;
    eth->h_dest[2] = DESTMAC2;
    eth->h_dest[3] = DESTMAC3;
    eth->h_dest[4] = DESTMAC4;
    eth->h_dest[5] = DESTMAC5;
    */
    eth->h_proto = htons(ETH_P_IP); //means next header will be IP header
    
    /* end of ethernet header */
    int total_len = 0; 
    total_len+=sizeof(struct ethhdr);

    struct iphdr *iph = (struct iphdr*)(sendbuff + sizeof(struct ethhdr));
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 16;
    iph->id = htons(10201);
    iph->ttl = 64;
    iph->protocol = 17;
    iph->saddr = inet_addr(inet_ntoa((((struct sockaddr_in *)&(ifreq_ip.ifr_addr))->sin_addr)));
    iph->daddr = inet_addr("127.0.0.1"); // put destination IP address
    
    total_len += sizeof(struct iphdr);

    struct udphdr *uh = (struct udphdr *)(sendbuff + sizeof(struct iphdr) + sizeof(struct ethhdr));
    
    uh->source = htons(23451);
    uh->dest = htons(23452);
    uh->check = 0;
    
    total_len+= sizeof(struct udphdr);

    sendbuff[total_len++] = 0xAA;
    sendbuff[total_len++] = 0xBB;
    sendbuff[total_len++] = 0xCC;
    sendbuff[total_len++] = 0xDD;
    sendbuff[total_len++] = 0xEE;

    uh->len = htons((total_len - sizeof(struct iphdr) - sizeof(struct ethhdr)));
    //UDP length field
    iph->tot_len = htons(total_len - sizeof(struct ethhdr));
    //IP length field
    iph->check = checksum((unsigned short*)(sendbuff + sizeof(struct ethhdr)), (sizeof(struct iphdr)/2));

    struct sockaddr_ll sadr_ll;
    sadr_ll.sll_ifindex = ifreq_i.ifr_ifindex; // index of interface
    sadr_ll.sll_halen = ETH_ALEN; // length of destination mac address
    for(int i = 0; i < 6; i++){
        sadr_ll.sll_addr[i] = destmac[i]; 
    }
    /*
    sadr_ll.sll_addr[0] = DESTMAC0;
    sadr_ll.sll_addr[1] = DESTMAC1;
    sadr_ll.sll_addr[2] = DESTMAC2;
    sadr_ll.sll_addr[3] = DESTMAC3;
    sadr_ll.sll_addr[4] = DESTMAC4;
    sadr_ll.sll_addr[5] = DESTMAC5;
    */
    int send_len = sendto(sock_raw,sendbuff,64,0,(const struct sockaddr*)&sadr_ll,sizeof(struct sockaddr_ll));
    if(send_len<0)
    {
        printf("error in sending....sendlen=%d....errno=%d\n",send_len,errno);
        return -1;
    }

    return 0; 
}