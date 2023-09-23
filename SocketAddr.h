#ifndef SocketAddr_H_H
#define SocketAddr_H_H

#include "Option.h"
#include "Result.h"
#include "slice/Slice.h"
#include "IpAddr.h"
#include <netinet/tcp.h>
#include <netinet/in.h>


class AddrParseError{

};

class SocketAddrV4{
    struct sockaddr_in ss;
public:
    static Result<SocketAddrV4> parse_ascii(Slice slice);
    //这是一个仅限夜间使用的实验性 API。( #101035addr_parse_ascii)
    //从字节片中解析 IPv4 套接字地址。
    SocketAddrV4();
    SocketAddrV4(struct sockaddr_in ss);
    SocketAddrV4(Ipv4Addr ip,  uint16_t port);
    SocketAddrV4(const SocketAddrV4 & addr);
    SocketAddrV4(SocketAddrV4 && addr);
    SocketAddrV4& operator =(const SocketAddrV4& addr);//赋值运算符


    Ipv4Addr ip();
    uint16_t port();
    void set_port(uint16_t port);

};
class SocketAddrV6{

};


class SocketAddr{
    bool is_v4;
    SocketAddrV4 V4;
    SocketAddrV6 V6;
public:
    SocketAddr();
    SocketAddr(SocketAddrV4 v4);
    SocketAddr(SocketAddrV6 v6);

    SocketAddr(const SocketAddr & addr);
    SocketAddr(SocketAddr && addr);

    SocketAddr& operator=(SocketAddr&& slice);
    SocketAddr& operator=(const SocketAddr& slice); 
};


#endif