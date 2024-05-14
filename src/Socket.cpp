
#include "Socket.h"
#ifdef _WIN32
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#include <stdlib.h>
#else
#include <errno.h>
#include <netdb.h>
#endif
#include <ctype.h>
#include <fcntl.h>

#include "IpAddr.h"

#ifdef _DEBUG
#define DEB(x) x; fflush(stderr);
#else
#define DEB(x)
#endif


// statics
#ifdef _WIN32
WSAInitializer Socket::m_winsock_init;
#endif




// only to be included in win32 projects
const char *StrError(int x)
{
    static	char tmp[100];
    switch (x)
    {
        case 10004: return "Interrupted function call.";
        case 10013: return "Permission denied.";
        case 10014: return "Bad address.";
        case 10022: return "Invalid argument.";
        case 10024: return "Too many open files.";
        case 10035: return "Resource temporarily unavailable.";
        case 10036: return "Operation now in progress.";
        case 10037: return "Operation already in progress.";
        case 10038: return "Socket operation on nonsocket.";
        case 10039: return "Destination address required.";
        case 10040: return "Message too long.";
        case 10041: return "Protocol wrong type for socket.";
        case 10042: return "Bad protocol option.";
        case 10043: return "Protocol not supported.";
        case 10044: return "Socket type not supported.";
        case 10045: return "Operation not supported.";
        case 10046: return "Protocol family not supported.";
        case 10047: return "Address family not supported by protocol family.";
        case 10048: return "Address already in use.";
        case 10049: return "Cannot assign requested address.";
        case 10050: return "Network is down.";
        case 10051: return "Network is unreachable.";
        case 10052: return "Network dropped connection on reset.";
        case 10053: return "Software caused connection abort.";
        case 10054: return "Connection reset by peer.";
        case 10055: return "No buffer space available.";
        case 10056: return "Socket is already connected.";
        case 10057: return "Socket is not connected.";
        case 10058: return "Cannot send after socket shutdown.";
        case 10060: return "Connection timed out.";
        case 10061: return "Connection refused.";
        case 10064: return "Host is down.";
        case 10065: return "No route to host.";
        case 10067: return "Too many processes.";
        case 10091: return "Network subsystem is unavailable.";
        case 10092: return "Winsock.dll version out of range.";
        case 10093: return "Successful WSAStartup not yet performed.";
        case 10101: return "Graceful shutdown in progress.";
        case 10109: return "Class type not found.";
        case 11001: return "Host not found.";
        case 11002: return "Nonauthoritative host not found.";
        case 11003: return "This is a nonrecoverable error.";
        case 11004: return "Valid name, no data record of requested type.";

        default:
            break;
    }
    snprintf(tmp, sizeof(tmp), "Winsock error code: %d", x);
    return tmp;
}



Socket::Socket(SOCKET _soket )
        :fd(_soket)
{
}


Socket::~Socket()
{
    if (fd == INVALID_SOCKET) // this could happen
    {
        //fprintf(stdout, "Socket::Close", 0, "file descriptor invalid", LOG_LEVEL_WARNING);
        return ;
    }
    int n;
    //Handler().ISocketHandler_Del(this); // remove from fd_set's
    if ((n = closesocket(fd)) == -1)
    {
        // failed...
        //fprintf(stdout, "close", Errno, StrError(Errno), LOG_LEVEL_ERROR);
    }
    fd = INVALID_SOCKET;
    return ;
}


SOCKET  Socket::Create(int af,int type, const std::string& protocol)
{
    struct protoent *p = NULL;

    if (protocol.size())
    {
        p = getprotobyname( protocol.c_str() );
        if (!p)
        {
            fprintf(stdout, "getprotobyname", Errno, StrError(Errno));
#ifdef ENABLE_EXCEPTIONS
            throw Exception(std::string("getprotobyname() failed: ") + StrError(Errno));
#endif
            return INVALID_SOCKET;
        }
    }

    int protno = p ? p -> p_proto : 0;

    SOCKET s = socket(af, type, protno);
    if (s == INVALID_SOCKET)
    {
        fprintf(stdout, "socket", Errno, StrError(Errno));
        close(s);
#ifdef ENABLE_EXCEPTIONS
        throw Exception(std::string("socket() failed: ") + StrError(Errno));
#endif
        return INVALID_SOCKET;;
    }
    //OnOptions(af, type, protno, s);

    return s;
}


SOCKET Socket::GetSocket()
{
    return fd;
}

bool Socket::IsIpv6()
{
    return m_ipv6;
}


/** Returns local port number for bound socket file descriptor. */
port_t Socket::GetSockPort()
{

    if (IsIpv6())
    {
        struct sockaddr_in6 sa;
        socklen_t sockaddr_length = sizeof(struct sockaddr_in6);
        if (getsockname(GetSocket(), (struct sockaddr *)&sa, (socklen_t*)&sockaddr_length) == -1)
            memset(&sa, 0, sizeof(sa));
        return ntohs(sa.sin6_port);
    }

    struct sockaddr_in sa;
    socklen_t sockaddr_length = sizeof(struct sockaddr_in);
    if (getsockname(GetSocket(), (struct sockaddr *)&sa, (socklen_t*)&sockaddr_length) == -1)
        memset(&sa, 0, sizeof(sa));
    return ntohs(sa.sin_port);
}

/** Returns local ipv4 address as text for bound socket file descriptor. */
IpAddr Socket::GetSockAddress()
{
    if (IsIpv6())
    {
        struct sockaddr_in6 sa;
        socklen_t sockaddr_length = sizeof(struct sockaddr_in6);
        if (getsockname(GetSocket(), (struct sockaddr *)&sa, (socklen_t*)&sockaddr_length) == -1)
            memset(&sa, 0, sizeof(sa));

        Ipv6Addr _addr( sa.sin6_addr );
        return IpAddr(_addr);
    }

    struct sockaddr_in sa;
    socklen_t sockaddr_length = sizeof(struct sockaddr_in);
    if (getsockname(GetSocket(), (struct sockaddr *)&sa, (socklen_t*)&sockaddr_length) == -1)
        memset(&sa, 0, sizeof(sa));
    Ipv4Addr addr( sa.sin_addr );

    return  IpAddr( addr );
}



#if 0
void inet_pton(int af, const char *src, void *dst)
{
	struct sockaddr_in ip4;
	struct sockaddr_in6 ip6;
	char ipaddr[64];
	sprintf(ipaddr,"%s:2020",src);
	int addr_size=-1;
	if (af == AF_INET){
		addr_size= sizeof(SOCKADDR_IN);;
		WSAStringToAddress( (LPSTR)ipaddr,af, NULL, (LPSOCKADDR)&ip4,&addr_size );
		memcpy(dst,&(ip4.sin_addr),4);
	} else if (af == AF_INET6){
		addr_size=sizeof(SOCKADDR_IN6);
		WSAStringToAddress( (LPSTR)ipaddr,af, NULL, (LPSOCKADDR)&ip6,&addr_size );
		memcpy(dst,&(ip6.sin6_addr),16);
	}
	//printf("ipaddr len=%d\nAF_INET6=%d,AF_INET=%d,af=%d\n",addr_size,AF_INET6,AF_INET,af);
}


const char* inet_ntop(int af, const void* src, char* dst, int cnt)
{
    wchar_t ip[INET6_ADDRSTRLEN];
    int len = INET6_ADDRSTRLEN;
    int ret = 1;

    if (af == AF_INET)
    {
        struct sockaddr_in in = {};
        in.sin_family = AF_INET;
        memcpy(&in.sin_addr, src, sizeof(struct in_addr));
        ret = WSAAddressToString((struct sockaddr*) &in, sizeof(struct sockaddr_in), 0, (char*)ip, (LPDWORD)&len);
    }
    else if (af == AF_INET6)
    {
        struct sockaddr_in6 in = {};
        in.sin6_family = AF_INET6;
        memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
        ret = WSAAddressToString((struct sockaddr*) &in, sizeof(struct sockaddr_in6), 0, (char*)ip, (LPDWORD)&len);
    }

    if (ret != 0)
    {
        return NULL;
    }

    if (!WideCharToMultiByte(CP_UTF8, 0, ip, len, dst, cnt, NULL, NULL))
    {
        return NULL;
    }

    return dst;
}
#endif

/** Returns remote port number: ipv4 and ipv6. */
port_t Socket::GetRemotePort()
{
    sockaddr_storage storage;
    socklen_t sock_len = sizeof(storage); // 锟斤拷锟斤拷锟斤拷锟街?
    int ret = ::getpeername(fd, (sockaddr *)&storage, &sock_len); // 注锟斤拷锟斤拷sock_id
    if (ret < 0){
        printf("getpeername error: %s\n", strerror(errno));
    }
    else{
        if (storage.ss_family == AF_INET){
            sockaddr_in *addr = (sockaddr_in *)&storage;
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr->sin_addr, ip, (int)sock_len);
            printf("client [%s:%d] \n", ip, ntohs(addr->sin_port));
            //Ipv4Addr(ip, ntohs(addr->sin_port));
            return  ntohs(addr->sin_port);
        }
        else if (storage.ss_family == AF_INET6){
            sockaddr_in6 *addr = (sockaddr_in6 *)&storage;
            char ip[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &addr->sin6_addr, ip, (int)sock_len);
            printf("client [%s:%d] \n", ip, ntohs(addr->sin6_port));
            //, ntohs(addr->sin6_port
            return  ntohs(addr->sin6_port);

        }
    }

    return 0;
}



Result<IpAddr, int> Socket::GetClientRemoteAddress()
{
    sockaddr_storage storage;
    socklen_t sock_len = sizeof(storage); // 锟斤拷锟斤拷锟斤拷锟街?
    int ret = ::getpeername(fd, (sockaddr *)&storage, &sock_len); // 注锟斤拷锟斤拷sock_id
    if (ret < 0){
        printf("getpeername error: %s\n", strerror(errno));
    }
    else{
        if (storage.ss_family == AF_INET){
            sockaddr_in *addr = (sockaddr_in *)&storage;
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr->sin_addr, ip, (int)sock_len);
            printf("client [%s:%d] \n", ip, ntohs(addr->sin_port));
            //Ipv4Addr(ip, ntohs(addr->sin_port));
            return  Ok( IpAddr(Ipv4Addr(addr->sin_addr)));
        }
        else if (storage.ss_family == AF_INET6){
            sockaddr_in6 *addr = (sockaddr_in6 *)&storage;
            char ip[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &addr->sin6_addr, ip, (int)sock_len);
            printf("client [%s:%d] \n", ip, ntohs(addr->sin6_port));
            //, ntohs(addr->sin6_port
            return  Ok( IpAddr(Ipv6Addr( addr->sin6_addr) ));

        }
    }

    return Err(Errno);
}





bool Socket::SetNonblocking(bool bNb)
{
#ifdef _WIN32
    unsigned long l = bNb ? 1 : 0;
    int n = ioctlsocket(fd, FIONBIO, &l);
    if (n != 0)
    {
        fprintf(stdout, "ioctlsocket(FIONBIO)", Errno, "");
        return false;
    }
    return true;
#else
    if (bNb)
	{
		if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		{
			fprintf(stdout,  "fcntl(F_SETFL, O_NONBLOCK)", Errno, StrError(Errno), LOG_LEVEL_ERROR);
			return false;
		}
	}
	else
	{
		if (fcntl(fd, F_SETFL, 0) == -1)
		{
			fprintf(stdout,  "fcntl(F_SETFL, 0)", Errno, StrError(Errno), LOG_LEVEL_ERROR);
			return false;
		}
	}
	return true;
#endif
}


bool Socket::SetNonblocking(bool bNb, SOCKET s)
{
#ifdef _WIN32
    unsigned long l = bNb ? 1 : 0;
    int n = ioctlsocket(s, FIONBIO, &l);
    if (n != 0)
    {
        fprintf(stdout,  "ioctlsocket(FIONBIO)", Errno, "");
        return false;
    }
    return true;
#else
    if (bNb)
	{
		if (fcntl(s, F_SETFL, O_NONBLOCK) == -1)
		{
			fprintf(stdout,  "fcntl(F_SETFL, O_NONBLOCK)", Errno, StrError(Errno), LOG_LEVEL_ERROR);
			return false;
		}
	}
	else
	{
		if (fcntl(s, F_SETFL, 0) == -1)
		{
			fprintf(stdout,  "fcntl(F_SETFL, 0)", Errno, StrError(Errno), LOG_LEVEL_ERROR);
			return false;
		}
	}
	return true;
#endif
}


/* IP options */
bool Socket::SetIpOptions(const void *p, socklen_t len)
{
#ifdef IP_OPTIONS
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_OPTIONS, (char *)p, len) == -1)
    {
        fprintf(stdout,  "setsockopt(IPPROTO_IP, IP_OPTIONS)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout,  "ip option not available", 0, "IP_OPTIONS", LOG_LEVEL_INFO);
	return false;
#endif
}


#ifdef IP_PKTINFO
bool Socket::SetIpPktinfo(bool x)
{
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_PKTINFO, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout,  "setsockopt(IPPROTO_IP, IP_PKTINFO)", Errno, StrError(Errno));
        return false;
    }
    return true;
}
#endif


#ifdef IP_RECVTOS
bool Socket::SetIpRecvTOS(bool x)
{
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_RECVTOS, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout, "setsockopt(IPPROTO_IP, IP_RECVTOS)", Errno, StrError(Errno));
        return false;
    }
    return true;
}
#endif


#ifdef IP_RECVTTL
bool Socket::SetIpRecvTTL(bool x)
{
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_RECVTTL, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout,  "setsockopt(IPPROTO_IP, IP_RECVTTL)", Errno, StrError(Errno));
        return false;
    }
    return true;
}
#endif


#ifdef IP_RECVOPTS
bool Socket::SetIpRecvopts(bool x)
{
	int optval = x ? 1 : 0;
	if (setsockopt(GetSocket(), IPPROTO_IP, IP_RECVOPTS, (char *)&optval, sizeof(optval)) == -1)
	{
		fprintf(stdout,  "setsockopt(IPPROTO_IP, IP_RECVOPTS)", Errno, StrError(Errno));
		return false;
	}
	return true;
}
#endif


#ifdef IP_RETOPTS
bool Socket::SetIpRetopts(bool x)
{
	int optval = x ? 1 : 0;
	if (setsockopt(GetSocket(), IPPROTO_IP, IP_RETOPTS, (char *)&optval, sizeof(optval)) == -1)
	{
		fprintf(stdout,  "setsockopt(IPPROTO_IP, IP_RETOPTS)", Errno, StrError(Errno));
		return false;
	}
	return true;
}
#endif


bool Socket::SetIpTOS(unsigned char tos)
{
#ifdef IP_TOS
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_TOS, (char *)&tos, sizeof(tos)) == -1)
    {
        fprintf(stdout,  "setsockopt(IPPROTO_IP, IP_TOS)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout,  "ip option not available", 0, "IP_TOS");
	return false;
#endif
}


unsigned char Socket::IpTOS()
{
    unsigned char tos = 0;
#ifdef IP_TOS
    socklen_t len = sizeof(tos);
    if (getsockopt(GetSocket(), IPPROTO_IP, IP_TOS, (char *)&tos, &len) == -1)
    {
        fprintf(stdout,  "getsockopt(IPPROTO_IP, IP_TOS)", Errno, StrError(Errno));
    }
#else
    fprintf(stdout,  "ip option not available", 0, "IP_TOS");
#endif
    return tos;
}


bool Socket::SetIpTTL(int ttl)
{
#ifdef IP_TTL
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(ttl)) == -1)
    {
        fprintf(stdout,  "setsockopt(IPPROTO_IP, IP_TTL)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "ip option not available", 0, "IP_TTL");
	return false;
#endif
}


int Socket::IpTTL()
{
    int ttl = 0;
#ifdef IP_TTL
    socklen_t len = sizeof(ttl);
    if (getsockopt(GetSocket(), IPPROTO_IP, IP_TTL, (char *)&ttl, &len) == -1)
    {
        fprintf(stdout, "getsockopt(IPPROTO_IP, IP_TTL)", Errno, StrError(Errno));
    }
#else
    fprintf(stdout,  "ip option not available", 0, "IP_TTL");
#endif
    return ttl;
}


bool Socket::SetIpHdrincl(bool x)
{
#ifdef IP_HDRINCL
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_HDRINCL, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout, "setsockopt(IPPROTO_IP, IP_HDRINCL)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "ip option not available", 0, "IP_HDRINCL");
	return false;
#endif
}


#ifdef IP_RECVERR
bool Socket::SetIpRecverr(bool x)
{
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_RECVERR, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout, "setsockopt(IPPROTO_IP, IP_RECVERR)", Errno, StrError(Errno));
        return false;
    }
    return true;
}
#endif


#ifdef IP_MTU_DISCOVER
bool Socket::SetIpMtudiscover(bool x)
{
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_MTU_DISCOVER, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout,  "setsockopt(IPPROTO_IP, IP_MTU_DISCOVER)", Errno, StrError(Errno));
        return false;
    }
    return true;
}
#endif


#ifdef IP_MTU
int Socket::IpMtu()
{
    int mtu = 0;
    socklen_t len = sizeof(mtu);
    if (getsockopt(GetSocket(), IPPROTO_IP, IP_MTU, (char *)&mtu, &len) == -1)
    {
        fprintf(stdout, "getsockopt(IPPROTO_IP, IP_MTU)", Errno, StrError(Errno));
    }
    return mtu;
}
#endif


#ifdef IP_ROUTER_ALERT
bool Socket::SetIpRouterAlert(bool x)
{
	int optval = x ? 1 : 0;
	if (setsockopt(GetSocket(), IPPROTO_IP, IP_ROUTER_ALERT, (char *)&optval, sizeof(optval)) == -1)
	{
		fprintf(stdout, "setsockopt(IPPROTO_IP, IP_ROUTER_ALERT)", Errno, StrError(Errno));
		return false;
	}
	return true;
}
#endif


bool Socket::SetIpMulticastTTL(int ttl)
{
#ifdef IP_MULTICAST_TTL
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl)) == -1)
    {
        fprintf(stdout, "setsockopt(IPPROTO_IP, IP_MULTICAST_TTL)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "ip option not available", 0, "IP_MULTICAST_TTL");
	return false;
#endif
}


int Socket::IpMulticastTTL()
{
    int ttl = 0;
#ifdef IP_MULTICAST_TTL
    socklen_t len = sizeof(ttl);
    if (getsockopt(GetSocket(), IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, &len) == -1)
    {
        fprintf(stdout, "getsockopt(IPPROTO_IP, IP_MULTICAST_TTL)", Errno, StrError(Errno));
    }
#else
    fprintf(stdout, "ip option not available", 0, "IP_MULTICAST_TTL");
#endif
    return ttl;
}


bool Socket::SetMulticastLoop(bool x)
{
#ifdef IP_MULTICAST_LOOP
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout, "setsockopt(IPPROTO_IP, IP_MULTICAST_LOOP)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "ip option not available", 0, "IP_MULTICAST_LOOP");
	return false;
#endif
}


#ifdef LINUX
bool Socket::IpAddMembership(struct ip_mreqn& ref)
{
#ifdef IP_ADD_MEMBERSHIP
	if (setsockopt(GetSocket(), IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ref, sizeof(struct ip_mreqn)) == -1)
	{
		fprintf(stdout, "setsockopt(IPPROTO_IP, IP_ADD_MEMBERSHIP)", Errno, StrError(Errno));
		return false;
	}
	return true;
#else
	fprintf(stdout, "ip option not available", 0, "IP_ADD_MEMBERSHIP");
	return false;
#endif
}
#endif


bool Socket::IpAddMembership(struct ip_mreq& ref)
{
#ifdef IP_ADD_MEMBERSHIP
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ref, sizeof(struct ip_mreq)) == -1)
    {
        fprintf(stdout,  "setsockopt(IPPROTO_IP, IP_ADD_MEMBERSHIP)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "ip option not available", 0, "IP_ADD_MEMBERSHIP", LOG_LEVEL_INFO);
	return false;
#endif
}


#ifdef LINUX
bool Socket::IpDropMembership(struct ip_mreqn& ref)
{
#ifdef IP_DROP_MEMBERSHIP
	if (setsockopt(GetSocket(), IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&ref, sizeof(struct ip_mreqn)) == -1)
	{
		fprintf(stdout, "setsockopt(IPPROTO_IP, IP_DROP_MEMBERSHIP)", Errno, StrError(Errno), LOG_LEVEL_FATAL);
		return false;
	}
	return true;
#else
	fprintf(stdout, "ip option not available", 0, "IP_DROP_MEMBERSHIP", LOG_LEVEL_INFO);
	return false;
#endif
}
#endif


bool Socket::IpDropMembership(struct ip_mreq& ref)
{
#ifdef IP_DROP_MEMBERSHIP
    if (setsockopt(GetSocket(), IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&ref, sizeof(struct ip_mreq)) == -1)
    {
        fprintf(stdout, "setsockopt(IPPROTO_IP, IP_DROP_MEMBERSHIP)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "ip option not available", 0, "IP_DROP_MEMBERSHIP", LOG_LEVEL_INFO);
	return false;
#endif
}


/* SOCKET options */


bool Socket::SetSoReuseaddr(bool x)
{
#ifdef SO_REUSEADDR
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_REUSEADDR)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_REUSEADDR", LOG_LEVEL_INFO);
	return false;
#endif
}


bool Socket::SetSoKeepalive(bool x)
{
#ifdef SO_KEEPALIVE
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_KEEPALIVE)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_KEEPALIVE", LOG_LEVEL_INFO);
	return false;
#endif
}


#ifdef SO_NOSIGPIPE
bool Socket::SetSoNosigpipe(bool x)
{
	int optval = x ? 1 : 0;
	if (setsockopt(GetSocket(), SOL_SOCKET, SO_NOSIGPIPE, (char *)&optval, sizeof(optval)) == -1)
	{
		fprintf(stdout, "setsockopt(SOL_SOCKET, SO_NOSIGPIPE)", Errno, StrError(Errno), LOG_LEVEL_FATAL);
		return false;
	}
	return true;
}
#endif


bool Socket::SoAcceptconn()
{
    int value = 0;
#ifdef SO_ACCEPTCONN
    socklen_t len = sizeof(value);
    if (getsockopt(GetSocket(), SOL_SOCKET, SO_ACCEPTCONN, (char *)&value, &len) == -1)
    {
        fprintf(stdout, "getsockopt(SOL_SOCKET, SO_ACCEPTCONN)", Errno, StrError(Errno));
    }
#else
    fprintf(stdout, "socket option not available", 0, "SO_ACCEPTCONN", LOG_LEVEL_INFO);
#endif
    return value ? true : false;
}


#ifdef SO_BSDCOMPAT
bool Socket::SetSoBsdcompat(bool x)
{
	int optval = x ? 1 : 0;
	if (setsockopt(GetSocket(), SOL_SOCKET, SO_BSDCOMPAT, (char *)&optval, sizeof(optval)) == -1)
	{
		fprintf(stdout, "setsockopt(SOL_SOCKET, SO_BSDCOMPAT)", Errno, StrError(Errno), LOG_LEVEL_FATAL);
		return false;
	}
	return true;
}
#endif


#ifdef SO_BINDTODEVICE
bool Socket::SetSoBindtodevice(const std::string& intf)
{
	if (setsockopt(GetSocket(), SOL_SOCKET, SO_BINDTODEVICE, (char *)intf.c_str(), intf.size()) == -1)
	{
		fprintf(stdout, "setsockopt(SOL_SOCKET, SO_BINDTODEVICE)", Errno, StrError(Errno), LOG_LEVEL_FATAL);
		return false;
	}
	return true;
}
#endif


bool Socket::SetSoBroadcast(bool x)
{
#ifdef SO_BROADCAST
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_BROADCAST, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_BROADCAST)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_BROADCAST", LOG_LEVEL_INFO);
	return false;
#endif
}


bool Socket::SetSoDebug(bool x)
{
#ifdef SO_DEBUG
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_DEBUG, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_DEBUG)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_DEBUG", LOG_LEVEL_INFO);
	return false;
#endif
}


int Socket::SoError()
{
    int value = 0;
#ifdef SO_ERROR
    socklen_t len = sizeof(value);
    if (getsockopt(GetSocket(), SOL_SOCKET, SO_ERROR, (char *)&value, &len) == -1)
    {
        fprintf(stdout, "getsockopt(SOL_SOCKET, SO_ERROR)", Errno, StrError(Errno));
    }
#else
    fprintf(stdout, "socket option not available", 0, "SO_ERROR", LOG_LEVEL_INFO);
#endif
    return value;
}


bool Socket::SetSoDontroute(bool x)
{
#ifdef SO_DONTROUTE
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_DONTROUTE, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_DONTROUTE)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_DONTROUTE", LOG_LEVEL_INFO);
	return false;
#endif
}


bool Socket::SetSoLinger(int onoff, int linger)
{
#ifdef SO_LINGER
    struct linger stl;
    stl.l_onoff = onoff;
    stl.l_linger = linger;
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_LINGER, (char *)&stl, sizeof(stl)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_LINGER)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_LINGER", LOG_LEVEL_INFO);
	return false;
#endif
}


bool Socket::SetSoOobinline(bool x)
{
#ifdef SO_OOBINLINE
    int optval = x ? 1 : 0;
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_OOBINLINE, (char *)&optval, sizeof(optval)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_OOBINLINE)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_OOBINLINE", LOG_LEVEL_INFO);
	return false;
#endif
}


#ifdef SO_PASSCRED
bool Socket::SetSoPasscred(bool x)
{
	int optval = x ? 1 : 0;
	if (setsockopt(GetSocket(), SOL_SOCKET, SO_PASSCRED, (char *)&optval, sizeof(optval)) == -1)
	{
		fprintf(stdout, "setsockopt(SOL_SOCKET, SO_PASSCRED)", Errno, StrError(Errno), LOG_LEVEL_FATAL);
		return false;
	}
	return true;
}
#endif


#ifdef SO_PEERCRED
bool Socket::SoPeercred(struct ucred& ucr)
{
	if (setsockopt(GetSocket(), SOL_SOCKET, SO_PEERCRED, (char *)&ucr, sizeof(ucr)) == -1)
	{
		fprintf(stdout, "setsockopt(SOL_SOCKET, SO_PEERCRED)", Errno, StrError(Errno), LOG_LEVEL_FATAL);
		return false;
	}
	return true;
}
#endif


#ifdef SO_PRIORITY
bool Socket::SetSoPriority(int x)
{
	if (setsockopt(GetSocket(), SOL_SOCKET, SO_PRIORITY, (char *)&x, sizeof(x)) == -1)
	{
		fprintf(stdout, "setsockopt(SOL_SOCKET, SO_PRIORITY)", Errno, StrError(Errno), LOG_LEVEL_FATAL);
		return false;
	}
	return true;
}
#endif


bool Socket::SetSoRcvlowat(int x)
{
#ifdef SO_RCVLOWAT
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_RCVLOWAT, (char *)&x, sizeof(x)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_RCVLOWAT)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_RCVLOWAT", LOG_LEVEL_INFO);
	return false;
#endif
}


bool Socket::SetSoSndlowat(int x)
{
#ifdef SO_SNDLOWAT
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_SNDLOWAT, (char *)&x, sizeof(x)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_SNDLOWAT)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_SNDLOWAT", LOG_LEVEL_INFO);
	return false;
#endif
}


bool Socket::SetSoRcvtimeo(struct timeval& tv)
{
#ifdef SO_RCVTIMEO
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_RCVTIMEO)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_RCVTIMEO", LOG_LEVEL_INFO);
	return false;
#endif
}


bool Socket::SetSoSndtimeo(struct timeval& tv)
{
#ifdef SO_SNDTIMEO
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_SNDTIMEO)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_SNDTIMEO", LOG_LEVEL_INFO);
	return false;
#endif
}


bool Socket::SetSoRcvbuf(int x)
{
#ifdef SO_RCVBUF
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_RCVBUF, (char *)&x, sizeof(x)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_RCVBUF)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_RCVBUF", LOG_LEVEL_INFO);
	return false;
#endif
}


int Socket::SoRcvbuf()
{
    int value = 0;
#ifdef SO_RCVBUF
    socklen_t len = sizeof(value);
    if (getsockopt(GetSocket(), SOL_SOCKET, SO_RCVBUF, (char *)&value, &len) == -1)
    {
        fprintf(stdout, "getsockopt(SOL_SOCKET, SO_RCVBUF)", Errno, StrError(Errno));
    }
#else
    fprintf(stdout, "socket option not available", 0, "SO_RCVBUF", LOG_LEVEL_INFO);
#endif
    return value;
}


#ifdef SO_RCVBUFFORCE
bool Socket::SetSoRcvbufforce(int x)
{
	if (setsockopt(GetSocket(), SOL_SOCKET, SO_RCVBUFFORCE, (char *)&x, sizeof(x)) == -1)
	{
		fprintf(stdout, "setsockopt(SOL_SOCKET, SO_RCVBUFFORCE)", Errno, StrError(Errno), LOG_LEVEL_FATAL);
		return false;
	}
	return true;
}
#endif


bool Socket::SetSoSndbuf(int x)
{
#ifdef SO_SNDBUF
    if (setsockopt(GetSocket(), SOL_SOCKET, SO_SNDBUF, (char *)&x, sizeof(x)) == -1)
    {
        fprintf(stdout, "setsockopt(SOL_SOCKET, SO_SNDBUF)", Errno, StrError(Errno));
        return false;
    }
    return true;
#else
    fprintf(stdout, "socket option not available", 0, "SO_SNDBUF", LOG_LEVEL_INFO);
	return false;
#endif
}


int Socket::SoSndbuf()
{
    int value = 0;
#ifdef SO_SNDBUF
    socklen_t len = sizeof(value);
    if (getsockopt(GetSocket(), SOL_SOCKET, SO_SNDBUF, (char *)&value, &len) == -1)
    {
        fprintf(stdout, "getsockopt(SOL_SOCKET, SO_SNDBUF)", Errno, StrError(Errno));
    }
#else
    fprintf(stdout, "socket option not available", 0, "SO_SNDBUF", LOG_LEVEL_INFO);
#endif
    return value;
}


#ifdef SO_SNDBUFFORCE
bool Socket::SetSoSndbufforce(int x)
{
	if (setsockopt(GetSocket(), SOL_SOCKET, SO_SNDBUFFORCE, (char *)&x, sizeof(x)) == -1)
	{
		fprintf(stdout, "setsockopt(SOL_SOCKET, SO_SNDBUFFORCE)", Errno, StrError(Errno));
		return false;
	}
	return true;
}
#endif


#ifdef SO_TIMESTAMP
bool Socket::SetSoTimestamp(bool x)
{
	int optval = x ? 1 : 0;
	if (setsockopt(GetSocket(), SOL_SOCKET, SO_TIMESTAMP, (char *)&optval, sizeof(optval)) == -1)
	{
		fprintf(stdout, "setsockopt(SOL_SOCKET, SO_TIMESTAMP)", Errno, StrError(Errno));
		return false;
	}
	return true;
}
#endif


int Socket::SoType()
{
    int value = 0;
#ifdef SO_TYPE
    socklen_t len = sizeof(value);
    if (getsockopt(GetSocket(), SOL_SOCKET, SO_TYPE, (char *)&value, &len) == -1)
    {
        fprintf(stdout, "getsockopt(SOL_SOCKET, SO_TYPE)", Errno, StrError(Errno));
    }
#else
    fprintf(stdout, "socket option not available", 0, "SO_TYPE", LOG_LEVEL_INFO);
#endif
    return value;
}


// void Socket::SetTimeout(time_t secs)
// {
// 	if (!secs)
// 	{
// 		m_timeout_start = 0;
// 		m_timeout_limit = 0;
// 		return;
// 	}
// 	m_timeout_start = time(NULL);
// 	m_timeout_limit = secs;
// 	Handler().SetTimeout();
// }


// bool Socket::CheckTimeout()
// {
// 	return m_timeout_start > 0 && m_timeout_limit > 0;
// }


// void Socket::OnTimeout()
// {
// }


// void Socket::OnConnectTimeout()
// {
// }


// bool Socket::Timeout(time_t tnow)
// {
// 	if (m_timeout_start > 0 && tnow - m_timeout_start > m_timeout_limit)
// 		return true;
// 	return false;
// }




