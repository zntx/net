#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
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
#include <netdb.h>
#include <iostream>
#include "IpAddr.h"

Ipv4Addr Ipv4Addr::create(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
}
Ipv4Addr Ipv4Addr::create(std::string ips)
{
}
Ipv4Addr Ipv4Addr::create(char *ips)
{
}

Ipv4Addr::Ipv4Addr(char *ips)
{
}

Slice Ipv4Addr::octets()
{

    return Slice(0, 0);
}
// 返回组成此地址的四个八位整数。

// 对于特殊的“未指定”地址 （） 返回 true。0.0.0.0
bool Ipv4Addr::is_unspecified()
{
    return true;
}
// 如果这是环回地址 （），则返回 true
bool Ipv4Addr::is_loopback()
{
    return true;
}

bool Ipv4Addr::is_private()
{
    return true;
}
// 如果这是专用地址，则返回 true。
/*
10.0.0.0/8
172.16.0.0/12
192.168.0.0/16
 */

bool Ipv4Addr::is_link_local()
{
    return true;
}
// 如果地址是链路本地 （），则返回 true。169.254.0.0/16
// 此属性由 IETF RFC 3927 定义。

bool is_multicast()
{

    return true;
}
// 如果这是多播地址，则返回 true（）。224.0.0.0/4
// 组播地址在 和 之间具有最重要的八位字节。 由 IETF RFC 5771 定义。224239
bool is_broadcast()
{

    return true;
}
// 如果这是广播地址，则返回 true（）。255.255.255.255
// 广播地址的所有八位字节都设置为 IETF RFC 919 中定义的八位字节。255

bool is_documentation()
{

    return true;
}
/*如果此地址在为文档指定的范围内，则返回 true。

这是在 IETF RFC 5737 中定义的：
192.0.2.0/24（测试网-1）
198.51.100.0/24（测试网-2）
203.0.113.0/24（测试网-3） */
