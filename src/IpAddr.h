#ifndef IPADDR_H_H
#define IPADDR_H_H
#include <stdint.h>
#include <arpa/inet.h>
#include "buff/buff.h"
#include "Result.h"

class Ipv4Addr{

public:
    struct in_addr sin;
    #define LOCALHOST "127.0.0.1"
    #define UNSPECIFIED "0.0.0.0"
    #define BROADCAST "255.255.25.255"
    static Result<Ipv4Addr, int> create(uint8_t a,uint8_t b,uint8_t c,uint8_t d);
    static Result<Ipv4Addr, int> create(std::string ips);
    static Result<Ipv4Addr, int> create(const char* ips);

    Ipv4Addr(struct in_addr &_sin);
    //返回组成此地址的四个八位整数。
    Slice octets();

    //对于特殊的“未指定”地址 （） 返回 true。0.0.0.0
    bool is_unspecified();

    //如果这是环回地址 （），则返回 true
    bool is_loopback();

    //如果这是专用地址，则返回 true。
    /*
    10.0.0.0/8
    172.16.0.0/12
    192.168.0.0/16
     */
    bool is_private();


    //如果地址是链路本地 （），则返回 true。169.254.0.0/16
    //此属性由 IETF RFC 3927 定义。
    bool is_link_local();

    //如果这是多播地址，则返回 true（）。224.0.0.0/4
    //组播地址在 和 之间具有最重要的八位字节。 由 IETF RFC 5771 定义。224239
    bool is_multicast();
    
    //如果这是广播地址，则返回 true（）。255.255.255.255
    //广播地址的所有八位字节都设置为 IETF RFC 919 中定义的八位字节。255
    bool is_broadcast();

    /*如果此地址在为文档指定的范围内，则返回 true。

    这是在 IETF RFC 5737 中定义的：
    192.0.2.0/24（测试网-1）
    198.51.100.0/24（测试网-2）
    203.0.113.0/24（测试网-3） */
    bool is_documentation();


};


class Ipv6Addr{
    
public:
    //char ips[INET6_ADDRSTRLEN];
    struct in6_addr sin6;
    static Result<Ipv6Addr, int> create(const char* ips);

    Ipv6Addr(struct in6_addr &sin6);

    /*如果这是环回地址 （），则返回 true
    是否为回环地址（loopback address）。回环地址是IPv6中的一种特殊地址类型，用于将数据发往本地主机。
    IPv6的回环地址是::1。
    */
    bool is_loopback();

    /* 否为链路本地地址（link-local address）。 
    用于在单个链路（例如：局域网）上进行通信。这些地址通常以fe80::/10的前缀开始。链路本地地址的前缀fe80::/10是预留给链路本地地址的，而不是全球唯一标识符。
    */
    bool is_link_local();

    /*是否为多播地址（multicast address）。
    多播地址是IPv6中的一种特殊地址类型，用于将数据传输给一组特定的接口。多播地址的前缀通常为ff00::/8 */
    bool is_broadcast();
};


class IpAddr{
public:
    bool is_v4;
    char ips[50];
    struct in_addr sin4;
    struct in6_addr sin6;

    IpAddr( Ipv4Addr &V4);
    IpAddr( Ipv6Addr &V6);
    ~IpAddr(  );

    static Result<IpAddr, int> create(const char* ips);
};

#endif