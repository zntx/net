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


/* IP地址合法性判断宏定义,只针对IPv4地址 */
#define NET_SWAP_32(x)						((((x)&0xff000000) >> 24)\
												| (((x)&0x00ff0000) >> 8)\
												| (((x)&0x0000ff00) << 8)\
												| (((x)&0x000000ff) << 24))
#define IN_IS_ADDR_NETMASK(mask)			(!((~(NET_SWAP_32(mask)) + 1) & (~(NET_SWAP_32(mask)))))
#define IN_IS_ADDR_LOOPBACK(ip)				(((ip)&0xff) == 0x7F)				//127
#define IN_IS_ADDR_MULTICAST(ip)			(((ip)&0xf0) == 0xE0)				//224~239
#define IN_IS_ADDR_RESERVE_MULTICAST(ip)	((((ip)&0xff) == 0xE0)\
												&& ((((ip)&0x00ff0000) >> 16) == 0)\
												&& ((((ip)&0x0000ff00) >> 8) == 0)\
												&& ((((ip)&0xff000000) >> 24) <= 0x12))//保留的多播地址，224.0.0.0~224.0.0.18
#define IN_IS_ADDR_BROADCAST(ip, mask)		(((ip)&(~(mask))) == (~(mask)))	//广播地址，即主机号全1
#define IN_IS_ADDR_NETADDR(ip, mask)		(!((ip)&(~(mask))))	//网络地址，即主机号全零
#define IN_IS_ADDR_UNSPECIFIED(ip)			(!(ip))	//地址为全零

Result<Ipv4Addr, int> Ipv4Addr::create(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    if ( a > 255 || b> 255 || c >255 || d > 255)
    {
        return Err(-1);
    }

    in_addr_t s_addr =   a *256*256*256 + b *256*256+ c * 256+ d;

	struct in_addr ip_a;
	ip_a.s_addr = htonl(s_addr);
    return Ok(Ipv4Addr(ip_a));
}
Result<Ipv4Addr, int> Ipv4Addr::create(std::string ips)
{
	return create(ips.c_str());
}

Result<Ipv4Addr, int> Ipv4Addr::create(const char *ips)
{
    struct in_addr ip_a;
    int ret = inet_pton(AF_INET, ips, &ip_a);
    if (ret <= 0)
    {
        return Err(errno);
    }
	
    Ipv4Addr v4(ip_a);
    return Ok(v4);
}


// Ipv4Addr::Ipv4Addr(const char *ips)
// {
//     struct in_addr ip_a;
//     int ret = inet_pton(AF_INET, ips, &ip_a);
//     if (ret <= 0)
//     {
//         //return Err(errno);
//     }
//     sin.s_addr = ip_a.s_addr;	
// }

Ipv4Addr::Ipv4Addr(struct in_addr &_sin)
{
	sin.s_addr = _sin.s_addr;
}

Slice Ipv4Addr::octets()
{
    return Slice(0, 0);
}
// 返回组成此地址的四个八位整数。

// 对于特殊的“未指定”地址 （） 返回 true。0.0.0.0
bool Ipv4Addr::is_unspecified()
{
    if( IN_IS_ADDR_UNSPECIFIED( &(this->sin) ) )
		return true;
	else
		return false;
}
// 如果这是环回地址 （），则返回 true
bool Ipv4Addr::is_loopback()
{
	if( IN_IS_ADDR_LOOPBACK( this->sin.s_addr ) )
		return true;
	else
		return false;
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

bool Ipv4Addr::is_multicast()
{

    return true;
}
// 如果这是多播地址，则返回 true（）。224.0.0.0/4
// 组播地址在 和 之间具有最重要的八位字节。 由 IETF RFC 5771 定义。224239
bool Ipv4Addr::is_broadcast()
{
	if( IN_IS_ADDR_MULTICAST( this->sin.s_addr ) )
		return true;
	else
		return false;
}
// 如果这是广播地址，则返回 true（）。255.255.255.255
// 广播地址的所有八位字节都设置为 IETF RFC 919 中定义的八位字节。255

bool Ipv4Addr::is_documentation()
{
    // if( IN_IS_ADDR_BROADCAST( this->sin.s_addr ) )
	// 	return true;
	// else
		return false;
}
/*如果此地址在为文档指定的范围内，则返回 true。

这是在 IETF RFC 5737 中定义的：
192.0.2.0/24（测试网-1）
198.51.100.0/24（测试网-2）
203.0.113.0/24（测试网-3） */

///


/*
*
*
*
*/




/**/
Result<Ipv6Addr, int> Ipv6Addr::create(const char *ips)
{
	struct in6_addr _sin;
    int ret = inet_pton(AF_INET6, ips, &_sin);
    if (ret == 0 || errno == EAFNOSUPPORT) {
        return Err(errno);
    }
	
	Ipv6Addr sin6(_sin);
	return Ok(sin6);
}


Ipv6Addr::Ipv6Addr(struct in6_addr &sin)
{
	memcpy(&sin6, &sin, sizeof(struct in6_addr));
}

bool Ipv6Addr::is_loopback()
{


    if (IN6_IS_ADDR_LOOPBACK(&this->sin6)) {
        //std::cout << "This is a link-local address" << std::endl;
		return true;
    } else {
       // std::cout << "This is not a link-local address" << std::endl;
	   return false;
    }
}

bool Ipv6Addr::is_link_local()
{
    if (IN6_IS_ADDR_LINKLOCAL( &this->sin6)) {
        //std::cout << "This is a link-local address" << std::endl;
		return true;
    } else {
       // std::cout << "This is not a link-local address" << std::endl;
	   return false;
    }
}

bool Ipv6Addr::is_broadcast()
{
    if (IN6_IS_ADDR_MULTICAST( &this->sin6)) {
        //std::cout << "This is a link-local address" << std::endl;
		return true;
    } else {
       // std::cout << "This is not a link-local address" << std::endl;
	   return false;
    }
}




/*
*
*
*/

IpAddr::IpAddr(Ipv4Addr &v4)
{
	sin4.s_addr = v4.sin.s_addr;
    is_v4 = true;
}
IpAddr::IpAddr(Ipv6Addr &v6)
{
    memcpy(&sin6  , &v6.sin6, sizeof(struct in6_addr));
    is_v4 = false;
}

IpAddr::~IpAddr()
{
}

Result<IpAddr, int> IpAddr::create(const char *host)
{
    int colons_numer = 0;

    for (int i = 0; i < strlen(host); i++)
    {
        if (host[i] == ':')
            colons_numer++;
    }
    if (colons_numer >= 2)
    {
        Result<Ipv6Addr, int> r_v6 = Ipv6Addr::create(host);
        if (r_v6.isOk())
        {
            Ipv6Addr v6 = r_v6.unwrap();
            return Ok(IpAddr(v6));
        }
        else
        {
            return Err(r_v6.unwrapErr());
        }
    }

    int point_number = 0;
    for (int i = 0; i < strlen(host); i++)
    {
        if (host[i] == '.')
            point_number++;
    }

    if (point_number == 3)
    {
        Result<Ipv4Addr, int> r_v4 = Ipv4Addr::create(host);
        if (r_v4.isOk())
        {
            Ipv4Addr v4 = r_v4.unwrap();
            return Ok(IpAddr(v4));
        }
        else
        {
            return Err(r_v4.unwrapErr());
        }
    }
}
