#include "ethernetHelper.hpp" 

const int EthernetHelper::size = 14; 

EthernetHelper::EthernetHelper (uint8_t srcMac[6], uint8_t destMac[6], uint16_t type){
    ptr = new struct ethhdr; 
    memcpy(ptr->h_dest, destMac, 6); 
    memcpy(ptr->h_source, srcMac, 6); 
    ptr->h_proto = htons(type);   
}

unsigned char* EthernetHelper::toArray(){
    return reinterpret_cast<unsigned char*>(ptr); 
}

void EthernetHelper::setType(uint16_t type){
    ptr->h_proto = htons(type); 
}