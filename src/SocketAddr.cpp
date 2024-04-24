#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_) || defined(WIN64) || defined(_WIN64) || defined(_WIN64_)
//Windows平台
#include <winsock2.h>
#include <in6addr.h>
#include <ws2tcpip.h>
#elif defined(ANDROID) || defined(_ANDROID_)
//Android平台
#elif defined(__linux__)
//Linux平台
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
//iOS、Mac平台
#else
//#define PLATFORM_UNKNOWN 1
#endif
#include "IpAddr.h"
#include "SocketAddr.h"


Result<SocketAddrV4, int> SocketAddrV4::parse_ascii(Slice<uint8_t> slice)
{
    return Err(1);
}




SocketAddrV4::SocketAddrV4(Ipv4Addr ip, uint16_t port)
{
    sa.sin_addr.s_addr = ip.sin.s_addr;
    sa.sin_port = htons(port);
}

SocketAddrV4::SocketAddrV4(struct sockaddr_in s)
{
    memcpy(&sa, &s, sizeof(struct sockaddr_in));
}

SocketAddrV4::SocketAddrV4(struct sockaddr_in *s)
{
    memcpy(&sa, s, sizeof(struct sockaddr_in));
}

SocketAddrV4::SocketAddrV4(const SocketAddrV4 &addr)
{
    memcpy(&sa, &addr.sa, sizeof(struct sockaddr_in));
}

SocketAddrV4::SocketAddrV4(SocketAddrV4 &&addr)
{
    memcpy(&sa, &addr.sa, sizeof(struct sockaddr_in));
}
SocketAddrV4 &SocketAddrV4::operator=(const SocketAddrV4 &addr) // 赋值运算�??
{
    memcpy(&sa, &addr.sa, sizeof(struct sockaddr_in));
}

Ipv4Addr SocketAddrV4::ip()
{
    return Ipv4Addr(this->sa.sin_addr);
}
uint16_t SocketAddrV4::port()
{
    return ntohs(this->sa.sin_port);
}
void SocketAddrV4::set_port(uint16_t port)
{
    this->sa.sin_port = htons(port);
}

std::string SocketAddrV4::to_string()
{
    return std::string(inet_ntoa(this->sa.sin_addr)) + ":" + std::to_string(this->port());
}

/*

*/
SocketAddrV6::SocketAddrV6(Ipv6Addr ip, uint16_t port)
{
    // ss.sin_addr.s_addr = inet_addr(ip.ips);
    // inet_pton_with_af(AF_INET6, ip.ips, &(ss.sin6_addr), sizeof(struct in6_addr));
    sa.sin6_port = htons(port);
    sa.sin6_family = AF_INET6;
}

SocketAddrV6::SocketAddrV6(struct sockaddr_in6 s)
{
    memcpy(&sa, &s, sizeof(struct sockaddr_in6));
}
SocketAddrV6::SocketAddrV6(struct sockaddr_in6 *s)
{
    memcpy(&sa, s, sizeof(struct sockaddr_in6));
}


std::string SocketAddrV6::to_string()
{
    char ip_str[INET6_ADDRSTRLEN];

    const void *src = &(this->sa.sin6_addr);
    const char *str = inet_ntop(AF_INET6, src, ip_str, INET6_ADDRSTRLEN); // 将数值格式转化为点分十进制的ip地址格式
    if (str == NULL)
    {
    }

    return std::string(ip_str) + ":" + std::to_string(this->sa.sin6_port);
}

/*

*/
static Result<SocketAddr, int> create(std::string ips, uint16_t port)
{
    // 判断IP 是V4还是V6
    Result<IpAddr, int> r_addr = IpAddr::create(ips.c_str());
    if (r_addr.isErr())
    {
        return Err(r_addr.unwrapErr());
    }

    IpAddr addr = r_addr.unwrap();

    if (addr.is_v4)
    {
        return Ok(SocketAddr(SocketAddrV4(addr.sin4, port)));
    }

    return Ok(SocketAddr(SocketAddrV6(addr.sin6, port)));
}

SocketAddr::SocketAddr(SocketAddrV4 v4)
{
    memcpy(&sin4, &v4.sa, sizeof(struct sockaddr_in));
    is_v4 = true;
}
SocketAddr::SocketAddr(SocketAddrV6 v6)
{
    memcpy(&sin6, &v6.sa, sizeof(struct sockaddr_in6));
    is_v4 = false;
}

SocketAddr::SocketAddr(const SocketAddr &addr)
{
    is_v4 = addr.is_v4;
    if (is_v4)
    {
        memcpy(&sin4, &addr.sin4, sizeof(struct sockaddr_in));
    }
    else
    {
        memcpy(&sin6, &addr.sin6, sizeof(struct sockaddr_in6));
    }
}
SocketAddr::SocketAddr(SocketAddr &&addr)
{
    this->is_v4 = addr.is_v4;
    if (this->is_v4)
    {
        memcpy(&this->sin4, &addr.sin4, sizeof(struct sockaddr_in));
    }
    else
    {
        memcpy(&this->sin6, &addr.sin6, sizeof(struct sockaddr_in6));
    }
}

SocketAddr &SocketAddr::operator=(SocketAddr &&addr)
{
    is_v4 = addr.is_v4;
    if (is_v4)
    {
        memcpy(&sin4, &addr.sin4, sizeof(struct sockaddr_in));
    }
    else
    {
        memcpy(&sin6, &addr.sin6, sizeof(struct sockaddr_in6));
    }
    return *this;
}
SocketAddr &SocketAddr::operator=(const SocketAddr &addr)
{
    is_v4 = addr.is_v4;
    if (is_v4)
    {
        memcpy(&sin4, &addr.sin4, sizeof(struct sockaddr_in));
    }
    else
    {
        memcpy(&sin6, &addr.sin6, sizeof(struct sockaddr_in6));
    }

    return *this;
}

std::string SocketAddr::to_string()
{
    char ip_str[INET6_ADDRSTRLEN];
    if (this->is_v4)
    {
        const void *src = &(this->sin4.sin_addr);
        const char *str = inet_ntop(AF_INET, src, ip_str, INET6_ADDRSTRLEN); // 将数值格式转化为点分十进制的ip地址格式
        if (str == NULL)
        {
        }

        return std::string(ip_str) + ":" + std::to_string(this->sin4.sin_port);

        // return std::string(inet_ntoa(this->sa.sin4.sin_addr)) + ":" + std::to_string(this->sa.sin4.sin_port);
    }
    else
    {
        const void *src = &(this->sin6.sin6_addr);
        const char *str = inet_ntop(AF_INET6, src, ip_str, INET6_ADDRSTRLEN); // 将数值格式转化为点分十进制的ip地址格式
        if (str == NULL)
        {
        }

        return std::string(ip_str) + ":" + std::to_string(this->sin6.sin6_port);
    }
}