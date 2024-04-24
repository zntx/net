#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_) || defined(WIN64) || defined(_WIN64) || defined(_WIN64_)
//Windowsƽ̨
#include <winsock2.h>
#include <in6addr.h>
#include <ws2tcpip.h>
#elif defined(ANDROID) || defined(_ANDROID_)
//Androidƽ̨
#elif defined(__linux__)
//Linuxƽ̨
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <linux/sockios.h>
#elif defined(__APPLE__) || defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_MAC)
//iOS��Macƽ̨
#else
//#define PLATFORM_UNKNOWN 1
#endif
#include <errno.h>






int main()
{   
    char* ips = "192.168.1.1";
    struct in_addr ip_a;
    int ret = inet_pton(AF_INET, ips, &ip_a);
    if (ret <= 0)
    {
        return errno;
    }


    printf(" addr %u %x\n", ip_a.s_addr , ip_a.s_addr);


}