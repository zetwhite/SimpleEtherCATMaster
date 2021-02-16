#include <iostream> 
#include <cstring>
#include <linux/if_ether.h>
#include <netinet/in.h>

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
#include<linux/if_packet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

#include "util/packet.hpp"
#include "util/util.hpp"
#include "layer2/ethernetHelper.hpp"
#include "layer3/ethercatHeader.hpp"

using namespace std;

void showUsage(string prog){
    cout << "[!] wrong usage [!]" << endl; 
    cout << "syntax : " << prog << " <interface> <dest mac>" << endl; 
    cout << "sample : " << prog << " wlan0 00:0a:95:9d:68:16" << endl; 
}

int main(int argc, char* argv[]){
    if(argc != 3){
        showUsage(argv[0]); 
    }

    uint8_t myMac[6]; 
    getMyMac(myMac, argv[1]); 

    uint8_t destMac[6]; 
    transformMac(destMac, argv[2]); 
    for(int i = 0; i <6; i++){
        cout << destMac[i] << " "; 
    }

    EthercatHelper ecathdr(0xd, 0, 1);
    EthernetHelper enethdr(myMac, destMac, 0x88a4); 

    Packet p(1518); 
    p.addFrame(enethdr.toArray(), enethdr.size);  
    p.addFrame(ecathdr.toArray(), ecathdr.size);    



    // ==== create socket and send ! 
    int sock_raw = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW); 
    if(sock_raw == -1)
        cout <<"error on socket\n"; 
    
    struct ifreq ifreq_i;
    memset(&ifreq_i,0,sizeof(ifreq_i));
    int interface_len =  strlen(argv[1]); 
    printf("%d\n", interface_len); 
    strncpy(ifreq_i.ifr_name,argv[1],interface_len); //giving name of Interface
    
    if((ioctl(sock_raw,SIOCGIFINDEX,&ifreq_i))<0){
        printf("error in index ioctl reading\n");//getting Index Name
        printf("error : %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }

    struct sockaddr_ll sadr_ll;
    sadr_ll.sll_ifindex = ifreq_i.ifr_ifindex; // index of interface
    sadr_ll.sll_halen = ETH_ALEN; // length of destination mac address
    //memcpy(sadr_ll.sll_addr, myMac, 6); 
    int send_len = sendto(sock_raw,p.buff,p.length, 0,(const struct sockaddr*)&sadr_ll,sizeof(struct sockaddr_ll));
    if(send_len<0)
    {
        printf("error in sending....sendlen=%d....errno=%d\n",send_len,errno);
        return -1;
    }


    return 0; 
}