#ifndef _ECAT_H_
#define _ECAT_H_

#include <stdint.h> 

struct ecathdr{
    uint16_t length:11; 
    uint16_t reserve:1; 
    uint16_t type:4; 
}; 

class EthercatHelper{
public:
    EthercatHelper(uint16_t len, bool reserve, uint8_t type); 
    unsigned char* toArray(); 
    void setLegnth(uint16_t len); 
    
    struct ecathdr* ptr; 
    static const int size; 
}; 

#endif