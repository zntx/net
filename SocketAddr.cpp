#include <iostream>
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
#include "SocketAddr.h"

Result<SocketAddrV4> SocketAddrV4::parse_ascii(Slice slice)
{
}

SocketAddrV4::SocketAddrV4(Ipv4Addr ip, uint16_t port)
{
    ss.sin_addr.s_addr = inet_addr(ip.ips);
    ss.sin_port = port;
}

SocketAddrV4::SocketAddrV4()
{
    memset(&ss, 0, sizeof(struct sockaddr_in));
}
SocketAddrV4::SocketAddrV4(const SocketAddrV4 &addr)
{
    memcpy(&ss, &addr.ss, sizeof(struct sockaddr_in));
}
SocketAddrV4::SocketAddrV4(SocketAddrV4 &&addr)
{
    memcpy(&ss, &addr.ss, sizeof(struct sockaddr_in));
}
SocketAddrV4& SocketAddrV4::operator =(const SocketAddrV4& addr)//赋值运算符
{
    memcpy(&ss, &addr.ss, sizeof(struct sockaddr_in));
}

Ipv4Addr SocketAddrV4::ip()
{
    return Ipv4Addr(inet_ntoa(this->ss.sin_addr));
}
uint16_t SocketAddrV4::port()
{
    return this->ss.sin_port;
}
void SocketAddrV4::set_port(uint16_t port)
{
    this->ss.sin_port = port;
}



SocketAddr::SocketAddr(const SocketAddr &addr)
{
    is_v4 = addr.is_v4;
    if (is_v4)
    {
        V4 = addr.V4;
    }
    else
    {
        V6 = addr.V6;
    }
}
SocketAddr::SocketAddr(SocketAddr &&addr)
{
    this->is_v4 = addr.is_v4;
    if (this->is_v4)
    {
        this->V4 = addr.V4;
    }
    else
    {
        this->V6 = addr.V6;
    }
}


SocketAddr& SocketAddr::operator=(SocketAddr&& addr)
{
    is_v4 = addr.is_v4;
    if (is_v4)
    {
        V4 = addr.V4;
    }
    else
    {
        V6 = addr.V6;
    }
    return *this;
}
SocketAddr& SocketAddr::operator=(const SocketAddr& addr)
{
    is_v4 = addr.is_v4;
    if (is_v4)
    {
        V4 = addr.V4;
    }
    else
    {
        V6 = addr.V6;
    }

    return *this;
}