#ifndef _SOCKUTIL_H_
#define _SOCKUTIL_H_

#include <sys/socket.h>
#include <net/if.h> 
#include <net/ethernet.h>
#include <arpa/inet.h>
#include "../util/packet.hpp"

class EthernetHelper{
public : 
    EthernetHelper(uint8_t srcMac[6], uint8_t destMac[6], uint16_t type); 
    unsigned char* toArray(); 
    void setType(uint16_t type); 

    struct ethhdr* ptr; 
    static const int size; 
}; 

#endif

