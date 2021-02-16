#ifndef _PACKET_H_
#define _PACKET_H_
#include <cstring> 

class Packet{
public: 
    unsigned char* buff; 
    int capacity; 
    int length; 
    Packet(int cap)
    : capacity(cap){
        buff = new unsigned char[cap]; 
        memset(buff, 0, cap); 
    }; 
    bool addFrame(unsigned char* frame, int size); 
}; 

#endif
