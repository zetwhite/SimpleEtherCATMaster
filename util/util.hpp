#ifndef _UTIL_H_
#define _UTIL_H_

#include <iostream>
#include <cstring>
#include <stdio.h> 

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

bool getMyMac(uint8_t macAddress[6],char* dev); 
bool transformMac(uint8_t destMac[6], char* srcMac); 
#endif