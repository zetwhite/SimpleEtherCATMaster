#include "packet.hpp" 

bool Packet::addFrame(unsigned char* frame, int size){
    if(capacity < length + size)
        return false; 
    memcpy(buff + length, frame, size); 
    length += size; 
    return true; 
}
