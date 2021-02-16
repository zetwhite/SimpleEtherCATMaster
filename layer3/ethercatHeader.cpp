#include "ethercatHeader.hpp"

const int EthercatHelper::size = 2; 

EthercatHelper::EthercatHelper(uint16_t len, bool reserve, uint8_t type){
    ptr = new struct ecathdr; 
    ptr->length = len; 
    ptr->reserve = reserve; 
    ptr->type = type; 
}

unsigned char* EthercatHelper::toArray(){
    return reinterpret_cast<unsigned char*>(ptr); 
}

void EthercatHelper::setLegnth(uint16_t len){
    ptr->length = len; 
}
