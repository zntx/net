#ifndef IPADDR_H_H
#define IPADDR_H_H
#include <stdint.h>
#include "slice/Slice.h"


class Ipv4Addr{

public:
    char ips[16];
    #define LOCALHOST "127.0.0.1"
    #define UNSPECIFIED "0.0.0.0"
    #define BROADCAST "255.255.25.255"
    static Ipv4Addr create(uint8_t a,uint8_t b,uint8_t c,uint8_t d);
    static Ipv4Addr create(std::string ips);
    static Ipv4Addr create(char* ips);

    Ipv4Addr(char* ips);


    Slice octets();
    //返回组成此地址的四个八位整数。

    //对于特殊的“未指定”地址 （） 返回 true。0.0.0.0
    bool is_unspecified();
    //如果这是环回地址 （），则返回 true
    bool is_loopback();

    bool is_private();
    //如果这是专用地址，则返回 true。
    /*
    10.0.0.0/8
    172.16.0.0/12
    192.168.0.0/16
     */

    bool is_link_local();
    //如果地址是链路本地 （），则返回 true。169.254.0.0/16
    //此属性由 IETF RFC 3927 定义。

    bool is_multicast();
    //如果这是多播地址，则返回 true（）。224.0.0.0/4
    //组播地址在 和 之间具有最重要的八位字节。 由 IETF RFC 5771 定义。224239
    bool is_broadcast();
    //如果这是广播地址，则返回 true（）。255.255.255.255
    //广播地址的所有八位字节都设置为 IETF RFC 919 中定义的八位字节。255

    bool is_documentation();
    /*如果此地址在为文档指定的范围内，则返回 true。

    这是在 IETF RFC 5737 中定义的：
    192.0.2.0/24（测试网-1）
    198.51.100.0/24（测试网-2）
    203.0.113.0/24（测试网-3） */

};


class Ipv6Addr{
    
};



#endif