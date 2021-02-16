#include "util.hpp"

bool getMyMac(uint8_t macAddress[6],char* dev){
    struct ifreq ifr;
    struct ifconf ifc;
    char buf[1024];
    int success = 0;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) { printf("socek error\n");};

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) { printf("getmac_ erro set ioctl\n");  }

    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

    for (; it != end; ++it) {
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                    success = 1;
                    break;
                }
            }
        }
        else { printf("getmac _ error ioctl\n"); }
    }

    if (success) 
        memcpy(macAddress, ifr.ifr_hwaddr.sa_data, 6);
    if (success) 
        return true; 
}

bool transformMac(uint8_t destMac[6], char* srcMac){
    std::cout << srcMac << std::endl; 
    sscanf(srcMac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &destMac[0], &destMac[1], &destMac[2], &destMac[3], &destMac[4], &destMac[5]);
    return true; 
}
