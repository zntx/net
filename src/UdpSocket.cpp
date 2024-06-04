#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include "UdpSocket.h"

Result<UdpSocket> UdpSocket::Connect(std::string domain)
{
    std::size_t pos = domain.find(":"); // position of "live" in str
    if (pos > 0)
    {
        std::string host = domain.substr(0, pos);   // get from "live" to the end
        std::string ports = domain.substr(pos + 1); // get from "live" to the end
        // std::cout << host << ' ' << ports << '\n';
        int port = atoi(ports.c_str());
        return UdpSocket::Connect(host.c_str(), port);
    }
    else
    {
        return UdpSocket::Connect(domain.c_str(), 80);
    }
}
#if 0

Result<UdpSocket,int> UdpSocket::connect (const char* host, size_t port)
{

    UdpSocket strem;
    ///定义sockfd
    strem.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (strem.fd < 0)
    {
        perror("setsockopet error\n");
        return Err(errno);
    }
    
    int reuse = 0;
    if (setsockopt(strem.fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopet error\n");
        return Err(errno);
    }
    ///定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(port);
    server_sockaddr.sin_addr.s_addr = inet_addr(host);
    
    ///bind，成功返�?0，出错返�?-1
    if(bind( strem.fd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) ==-1)
    {
        perror("bind");
        return Err(errno);
    }
    
    ///listen，成功返�?0，出错返�?-1
    if(listen(strem.fd, 24) == -1)
    {
        perror("listen");
        exit(1);
    }

    return Ok(strem);
}

#endif

Result<UdpSocket> UdpSocket::Connect(const char *host, size_t port)
{
    char host_ip[50] = {0};

    struct hostent *_hostent = gethostbyname(host);
    if (_hostent != nullptr)
    {
        for (int i = 0; _hostent->h_addr_list[i] != NULL; i++)
        {
            char *ipaddr = inet_ntoa(*((struct in_addr *)_hostent->h_addr_list[i]));
            if (strlen(host_ip) == 0 && ipaddr != nullptr)
                strncpy(host_ip, ipaddr, strlen(ipaddr));
            printf("ip addr%d: %s\n", i + 1, ipaddr);
        }
    }
    if (strlen(host_ip) == 0)
        strncpy(host_ip, host, strlen(host));

    // 判断IP 是V4还是V6
    auto r_addr = IpAddr::create(host_ip);
    if (r_addr.isErr())
    {
        return Err(r_addr.unwrapErr());
    }

    IpAddr addr = r_addr.unwrap();

    int strem = 0;
    if (addr.is_v4)
    {
        strem = socket(AF_INET, SOCK_DGRAM, 0);
        if (strem < 0)
        {
            perror("setsockopet error\n");
            return Err(std::string(StrError(Errno)));
        }

        struct sockaddr_in server_sockaddr;
        server_sockaddr.sin_family = AF_INET;
        server_sockaddr.sin_port = htons(port);
        server_sockaddr.sin_addr.s_addr = inet_addr(host_ip);

        int ret = connect(strem, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr));
        if (ret < 0)
        {
            perror("connect");
            return Err(std::string(StrError(Errno)));
        }
    }
    else
    {
        strem = socket(AF_INET6, SOCK_STREAM, 0);
        if (strem < 0)
        {
            perror("setsockopet error\n");
            return Err(std::string(StrError(Errno)));
        }

        struct sockaddr_in6 server_sockaddr6;
        server_sockaddr6.sin6_family = AF_INET6;
        int ret = connect(strem, (struct sockaddr *)&server_sockaddr6, (socklen_t)sizeof(struct sockaddr_in6));
        if (ret < 0)
        {
            perror("connect");
            return Err(std::string(StrError(Errno)));
        }
    }

    return Ok(UdpSocket(strem));
}

Result<UdpSocket> Connect_timeout(SocketAddr &addr, std::chrono::duration<int, std::ratio<1, 2>> timeout)
{
    char host_ip[50] = {0};

    int msecond = 0;

    int sockfd = 0;

    // int socket_fd = socket(p_addr_res->ai_family, p_addr_res->ai_socktype, p_addr_res->ai_protocol);
    // if(socket_fd < 0)
    // {
    //     continue;
    // }


#ifdef UNIX
    int block_or_not = 1; // 设置非阻塞
    if (ioctlsocket(sockfd, FIONBIO, (char *)&on) < 0)
#endif

#ifdef WIN32
    unsigned long on_windows=1;
    if (ioctlsocket(sockfd, FIONBIO, &on_windows) < 0)
#endif


#ifdef VOS
        int off=0;
    if (ioctlsocket(sockfd, FIONBIO, (char *)&off) <0)
#endif
    {
        // NETBASE_ERR("ioctl socket failed\n");
        return Err(std::string(StrError(Errno)));
    }

    int ret_val = -1; // 接收函数返回
    if (addr.is_v4)
    {
        ret_val = connect(sockfd, (struct sockaddr *)&addr.sin4, (socklen_t)sizeof(struct sockaddr_in));
    }
    else
    {
        ret_val = connect(sockfd, (struct sockaddr *)&addr.sin6, (socklen_t)sizeof(struct sockaddr_in6));
    }
    if (-1 == ret_val)
    {
        if (EINPROGRESS == errno)
        {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(sockfd, &set);

            struct timeval mytm;
            memset(&mytm, 0, sizeof(struct timeval));
            mytm.tv_sec = msecond / 1000;
            mytm.tv_usec = (msecond % 1000) * 1000;

            if (select(sockfd + 1, NULL, &set, NULL, &mytm) > 0)
            {
                char err = 0; // error�?
                int len = sizeof(int);
                // 清除错�??
                (void)getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, (socklen_t *)&len);
                if (0 == err)
                {
                    ret_val = 0;
                }
                else
                {
                    ret_val = -1;
                }
            }
            else
            {
                ret_val = -1;
            }
        }
    }

#ifdef UNIX
    block_or_not = 0; // 设置非阻塞
    if (ioctlsocket(sockfd, FIONBIO, (char *)&on) < 0)
#endif

#ifdef WIN32
    on_windows=0;
    if (ioctlsocket(sockfd, FIONBIO, &on_windows) < 0)
#endif

#ifdef VOS
        int off=1;
    if (ioctlsocket(sockfd, FIONBIO, (char *)&off) <0)
#endif
    {
        // NETBASE_ERR("ioctl socket failed\n");
        return Err(std::string(StrError(Errno)));
    }

    return Ok(UdpSocket(sockfd));
}

UdpSocket::UdpSocket(int fd)
{
    this->fd = fd;
}

size_t UdpSocket::read(Slice<uint8_t> slice)
{
    return recv(this->fd, (char*)slice.addr, slice.len, 0);
}

size_t UdpSocket::write(Slice<uint8_t> slice)
{
    return send(this->fd, (char*)slice.addr, slice.len, 0);
}

Result<SocketAddr> UdpSocket::peer_addr()
{
    struct sockaddr_in peerAddr;

    int32_t peerLen = sizeof(peerAddr);
    if (getpeername(this->fd, (struct sockaddr *)&peerAddr, &peerLen) == -1)
    {
        printf("getpeername error\n");
        // string str = strerror(errno);
        // return Err(str);
    }

    return Ok(SocketAddr(SocketAddrV4(peerAddr)));
}

Result<SocketAddr> UdpSocket::local_addr()
{
    struct sockaddr_in connectedAddr;
    int32_t len = sizeof(connectedAddr);
    if (getsockname(this->fd, (struct sockaddr *)&connectedAddr, &len) == -1)
    {
        printf("getsockname error\n");
        // string str = strerror(errno);
        // return Err(str);
    }

    // SocketAddr addr(SocketAddrV4(connectedAddr));
    return Ok(SocketAddr(SocketAddrV4(connectedAddr)));
}

// Result<void, string> UdpSocket::set_read_timeout(struct timeval *tv)
// {
//     if( tv != NULL) {
//         socklen_t optlen = sizeof(struct timeval);
//         //getsockopt(this->fd, SOL_SOCKET,SO_RCVTIMEO, &tv, &optlen);
//         setsockopt(this->fd, SOL_SOCKET, SO_RCVTIMEO, tv, optlen);
//     }
//     else
//     {
//         struct timeval tv;
//         tv.tv_sec = 10; tv.tv_usec = 0;
//         socklen_t optlen = sizeof(struct timeval);
//         //getsockopt(this->fd, SOL_SOCKET,SO_RCVTIMEO, &tv, &optlen);
//         setsockopt(this->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, optlen);
//     }

//     return Ok();
// }

// Result<void, string> UdpSocket::set_write_timeout(struct timeval *tv)
// {
//     if( tv != NULL) {
//         socklen_t optlen = sizeof(struct timeval);
//         //getsockopt(this->fd, SOL_SOCKET,SO_RCVTIMEO, &tv, &optlen);
//         setsockopt(this->fd, SOL_SOCKET, SO_SNDTIMEO, tv, optlen);
//     }
//     else
//     {
//         struct timeval tv;
//         tv.tv_sec = 10; tv.tv_usec = 0;
//         socklen_t optlen = sizeof(struct timeval);
//         //getsockopt(this->fd, SOL_SOCKET,SO_RCVTIMEO, &tv, &optlen);
//         setsockopt(this->fd, SOL_SOCKET, SO_SNDTIMEO, &tv, optlen);
//     }

//     return Ok();
// }

Result<Option<struct timeval>, int> UdpSocket::read_timeout()
{

    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    socklen_t optlen = sizeof(struct timeval);
    // getsockopt(this->fd, SOL_SOCKET,SO_RCVTIMEO, &tv, &optlen);
    setsockopt(this->fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, optlen);

    Option<struct timeval> ret = Some(tv);
    return Ok(std::move(ret));
}
Result<Option<struct timeval>, int> UdpSocket::write_timeout()
{
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    socklen_t optlen = sizeof(struct timeval);
    // getsockopt(this->fd, SOL_SOCKET,SO_RCVTIMEO, &tv, &optlen);
    setsockopt(this->fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, optlen);

    Option<struct timeval> ret = Some(tv);
    return Ok(std::move(ret));
}

// Result<void, string> UdpSocket::set_nodelay(bool _nodelay  )
// {
//     int nodelay = 1;
//     int cork = 1;

//     if( _nodelay) {
//         nodelay = 1;
//         cork = 1;
//     }else {
//         nodelay = 0;
//         cork =0;
//     }

// 	int rc = setsockopt(this->fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(int));
// 	rc = setsockopt(this->fd, IPPROTO_TCP, TCP_CORK, &cork, sizeof(int));

//     return Ok();
// }

Result<bool, int> UdpSocket::nodelay()
{
    char nodelay = 0;
    int rc = setsockopt(this->fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(int));

    if (nodelay)
        return Ok(true);
    else
        return Ok(false);
}

// Result<void, string> UdpSocket::set_nonblocking(bool nonblocking)
// {
//     int block_or_not = 1; // 设置非阻�?
//     if(nonblocking)
//         block_or_not = 1;
//     else
//         block_or_not = 0;
// 	if( ioctl(this->fd, FIONBIO, &block_or_not) != 0)
// 	{
// 		string str(strerror(errno));
//         return Err(str);
// 	}
//     return Ok();
// }

// Result<void, string> UdpSocket::set_ttl(uint32_t ttl  )
// {

//     return Ok();
// }
Result<uint32_t, int> UdpSocket::ttl()
{
    uint32_t x = 3;
    return Ok(std::move(x));
}