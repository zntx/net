#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include <iostream>
#include "socket_include.h"
#include "TcpStream.h"

Result<TcpStream> TcpStream::Connect(std::string domain)
{
    std::size_t pos = domain.find(":"); // position of "live" in str
    if (pos != string::npos)
    {
        std::string host = domain.substr(0, pos);   // get from "live" to the end
        std::string ports = domain.substr(pos + 1); // get from "live" to the end

        int port = atoi(ports.c_str());
        
        auto tmp = TcpStream::Connect(Slice<const char>(host.c_str(),host.length()), port);
        
        return tmp;
    }
    else
    {
        std::cout << " call 00000-1" << std::endl;
        return TcpStream::Connect(Slice<const char>(domain.c_str(),domain.length()), 80);
    }
}
#if 0

Result<TcpStream,int> TcpStream::connect (const char* host, size_t port)
{

    TcpStream strem;
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
    
    ///bind，成功返??0，出错返??-1
    if(bind( strem.fd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) ==-1)
    {
        perror("bind");
        return Err(errno);
    }
    
    ///bind，成功返回0，出错返回-1
    if(listen(strem.fd, 24) == -1)
    {
        perror("listen");
        exit(1);
    }

    return Ok(strem);
}

#endif

Result<TcpStream> TcpStream::Connect(Slice<const char> host, size_t port)
{
    SOCKET stream = INVALID_SOCKET;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags =  AI_ALL; //AI_NUMERICHOST; //
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;
    struct addrinfo *res = nullptr;
    //if (Utility::isipv6(host))
    //    hints.ai_flags |= AI_NUMERICHOST;

    std::cout << "Connect host :" << host.to_string() << "."<<std::endl;
    int n = getaddrinfo(host.to_string().c_str(), nullptr, &hints, &res);
    if (!n)
    {
        for (struct addrinfo *ai = res ; ai != nullptr; ai = ai -> ai_next)
        {
            std::cout << "ip_addr :" <<  ai -> ai_protocol << "."<<std::endl;

            struct sockaddr_in *sockaddr_ipv4 = (struct sockaddr_in *) ai->ai_addr;
            printf("\tIPv4 address %s\n",
                   inet_ntoa(sockaddr_ipv4->sin_addr) );

            auto ip_addr =  SocketAddr(ai->ai_addr);

            ip_addr.set_port(port);

            std::cout << "ip_addr :" << ip_addr.to_string() << "." << std::endl;

            auto client = Connect(ip_addr);
            if( client.is_err())
                continue;

            stream = client.unwrap().take();
            std::cout << "find "<<std::endl;
            break;

        }
        freeaddrinfo(res);
    }
    else
    {
        std::cout << "getaddrinfo error :" <<std::endl;
        auto _ips =  SocketAddr::Create( host, port);
        if( _ips.is_err()){
            return Err(_ips.unwrap_err());
        }

        auto ips= _ips.unwrap();
        ips.set_port(port);

        std::cout << "ips :" << ips.to_string() << "."<<std::endl;

        auto client = Connect(ips);
        if( client.is_err())
            return Err(_ips.unwrap_err());

        stream = client.unwrap().take();

    }

    if( stream == INVALID_SOCKET)
        return Err(std::string(StrError(Errno)));

    return Ok(TcpStream(stream));
}


Result<TcpStream> TcpStream::Connect(SocketAddr &addr)
{
    SOCKET stream = INVALID_SOCKET;

    int family = addr.is_v4 ? AF_INET : AF_INET6;
    printf("family %d\n", family);
    auto client = Socket::Create(family, SOCK_STREAM, IPPROTO_TCP);
    if( client.is_err())
    {
        printf("Socket::Create error\n");
        return Err(client.unwrap_err());
    }

    auto _client  = client.unwrap();

    auto ret = _client.connect(addr);

    if( ret.is_err())
    {
        return Err(ret.unwrap_err());
    }

    return Ok(TcpStream(_client.take()));
}


Result<TcpStream> Connect_timeout(SocketAddr &addr, std::chrono::duration<int, std::ratio<1, 2>> timeout)
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
    int block_or_not = 1; // 设置非阻
    if (ioctlsocket(sockfd, FIONBIO, (char *)&on) < 0)
#endif
#ifdef __WINDOWS__
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
    if (0 != ret_val)
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
                char err = 0; // error??
                int len = sizeof(int);
                // 清除错误
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
    block_or_not = 0; // 设置阻塞
    if (ioctlsocket(sockfd, FIONBIO, (char *)&on) < 0)
#endif
#ifdef __WINDOWS__
    on_windows=1;
    if (ioctlsocket(sockfd, FIONBIO, &on_windows) < 0)
#endif
#ifdef VOS
        off=0;
    if (ioctlsocket(sockfd, FIONBIO, (char *)&off) <0)
#endif
    {
        // NETBASE_ERR("ioctl socket failed\n");
        return Err(std::string(StrError(Errno)));
    }

    return Ok(TcpStream(sockfd));
}



Result<TcpStream> TcpStream::Connect_timeout(std::string domain, struct timeval timeout)
{
    std::size_t pos = domain.find(":"); // position of "live" in str
    if (pos > 0)
    {
        std::string host = domain.substr(0, pos);   // get from "live" to the end
        std::string ports = domain.substr(pos + 1); // get from "live" to the end
        std::cout << host << ' ' << ports << '\n';
        int port = atoi(ports.c_str());

        auto tmp = TcpStream::Connect_timeout(Slice<const char>(host.c_str(),host.length()), port, timeout);

        return tmp;
    }
    else
    {
        std::cout << " call 00000-1" << std::endl;
        return TcpStream::Connect_timeout(Slice<const char>(domain.c_str(),domain.length()), 80, timeout);
    }
}
#if 0

Result<TcpStream,int> TcpStream::connect (const char* host, size_t port)
{

    TcpStream strem;
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

    ///bind，成功返??0，出错返??-1
    if(bind( strem.fd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) ==-1)
    {
        perror("bind");
        return Err(errno);
    }

    ///bind，成功返回0，出错返回-1
    if(listen(strem.fd, 24) == -1)
    {
        perror("listen");
        exit(1);
    }

    return Ok(strem);
}

#endif

Result<TcpStream> TcpStream::Connect_timeout(Slice<const char> host, size_t port, struct timeval timeout)
{
    SOCKET stream = INVALID_SOCKET;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags =  AI_ALL; //AI_NUMERICHOST; //
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;
    struct addrinfo *res = nullptr;
    //if (Utility::isipv6(host))
    //    hints.ai_flags |= AI_NUMERICHOST;

    std::cout << "Connect host :" << host.to_string() << "."<<std::endl;
    int n = getaddrinfo(host.to_string().c_str(), nullptr, &hints, &res);
    if (!n)
    {
        for (struct addrinfo *ai = res ; ai != nullptr; ai = ai -> ai_next)
        {
            std::cout << "ip_addr :" <<  ai -> ai_protocol << "."<<std::endl;

            struct sockaddr_in *sockaddr_ipv4 = (struct sockaddr_in *) ai->ai_addr;
            printf("\tIPv4 address %s\n",
                   inet_ntoa(sockaddr_ipv4->sin_addr) );

            auto ip_addr =  SocketAddr(ai->ai_addr);

            ip_addr.set_port(port);

            std::cout << "ip_addr :" << ip_addr.to_string() << "." << std::endl;

            auto client = Connect_timeout(ip_addr, timeout);
            if( client.is_err())
                continue;

            stream = client.unwrap().take();
            std::cout << "find "<<std::endl;
            break;

        }
        freeaddrinfo(res);
    }
    else
    {
        std::cout << "getaddrinfo error :" <<std::endl;
        auto _ips =  SocketAddr::Create( host, port);
        if( _ips.is_err()){
            return Err(_ips.unwrap_err());
        }

        auto ips= _ips.unwrap();
        ips.set_port(port);

        std::cout << "ips :" << ips.to_string() << "."<<std::endl;

        auto client = Connect_timeout(ips, timeout);
        if( client.is_err())
            return Err(_ips.unwrap_err());

        stream = client.unwrap().take();

    }

    if( stream == INVALID_SOCKET)
        return Err(std::string(StrError(Errno)));

    return Ok(TcpStream(stream));
}


Result<TcpStream> TcpStream::Connect_timeout(SocketAddr &addr, struct timeval timeout)
{
    int family = addr.is_v4 ? AF_INET : AF_INET6;
    printf("family %d\n", family);
    auto client = Socket::Create(family, SOCK_STREAM, IPPROTO_TCP);
    if( client.is_err())
    {
        printf("Socket::Create error\n");
        return Err(client.unwrap_err());
    }

    auto _client  = client.unwrap();

    // 清除错误
    int opt_val = 0;
    int length = sizeof(opt_val);
    auto socket_fd = _client.get_socket( );

    _client.set_nodelay(1);

    printf("time start %lu\n", time(nullptr));
    auto ret = _client.connect(addr);
    if( ret.is_err())
    {
#ifdef  __WINDOWS__
        if( Errno == WSAEWOULDBLOCK)
#else
        if( Errno == EINPROGRESS)
#endif
        {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(socket_fd, &set);

            if(::select(socket_fd + 1, NULL, &set, NULL, &timeout) > 0)
            {
                getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, (char*)&opt_val, (socklen_t*)&length);
            }
        }
    }
    printf("time end %lu\n", time(nullptr));
    _client.set_nodelay(0);

    if (0 == opt_val)
    {
        return Ok(TcpStream(_client.take()));
    }
    else{
        return Err(client.unwrap_err());
    }
}


TcpStream::~TcpStream()
{
    if (this->fd != INVALID_SOCKET) {
        std::cout << "~TcpStream():  close() " << this->fd << std::endl;
        close(this->fd);

        this->fd= INVALID_SOCKET;
    }
}

TcpStream::TcpStream(int fd)
{
    this->fd = fd;
}

TcpStream::TcpStream(TcpStream &&other)
{
    this->fd = other.fd;
    other.fd = INVALID_SOCKET;
}

//TcpStream::TcpStream(const TcpStream &other)
//{
//    this->fd = other.fd;
//    // other.fd = -1;
//}


TcpStream &TcpStream::operator=(TcpStream &&other)
{
    this->fd = other.fd;
    other.fd = -1;

    return *this;
}

size_t TcpStream::read(Slice<uint8_t> &slice)
{
    return recv(this->fd, (char*)slice.addr, slice.len, 0);
}

size_t TcpStream::read(Slice<char> &slice)
{
    return recv(this->fd, slice.addr, slice.len, 0);
}

size_t TcpStream::write(Slice<uint8_t> &slice)
{
    return send(this->fd,  (char*)slice.addr, slice.len, 0);
}

Result<TcpListener> TcpListener::bin(const char *host_name, uint16_t port)
{
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // AF_INET6;  

    int status = getaddrinfo(host_name, NULL, &hints, &result);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return Err(std::string(StrError(Errno)));
    }

    int stream = -1;
  
    for (struct addrinfo *p = result; p != NULL; p = p->ai_next)
    {
        if (p->ai_family == AF_INET)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;

            ipv4->sin_port = htons(port);
            SocketAddrV4 addrv4(ipv4);

            SocketAddr addr = SocketAddr(addrv4);

            stream = socket(AF_INET, SOCK_STREAM, 0);
            if (stream < 0)
            {
                fprintf(stdout, "%s:%d socket failed: errno %d %s\n", __FILE__, __LINE__, errno, strerror(errno));
                break;
            }

            char tmp = 1;
            int ret = setsockopt(stream, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp));
            if (ret < 0)
            {
                fprintf(stdout, "%s:%d setsockopt failed: errno %d %s\n", __FILE__, __LINE__, errno, strerror(errno));
            }

            //printf(" addr.sa.sin4. port %d\n", addr.sa.sin4.sin_port);

            //std::cout << "addr " << addr.to_string() << std::endl;

            ret = ::bind(stream, (struct sockaddr *)&(addr.sin4), sizeof(struct sockaddr_in));
            if (ret < 0)
            {
                fprintf(stdout, "%s:%d bind failed: errno %d %s\n", __FILE__, __LINE__, errno, strerror(errno));
                close(stream);
                stream = -1;
                break;
            }

            break;
        }
        else if (p->ai_family == AF_INET6)
        {
            // IPv6???
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            ipv6->sin6_port = port;

            SocketAddrV6 addrv6(ipv6);
            SocketAddr addr(addrv6);
            // addr = &(ipv6->sin6_addr);
            // ipver = "IPv6";
            stream = socket(AF_INET6, SOCK_STREAM, 0);
            if (stream < 0)
            {
                fprintf(stdout, "%s:%d socket failed: errno %d %s\n", __FILE__, __LINE__, errno, strerror(errno));
                break;
            }

            char tmp = 1;
            int ret = setsockopt(stream, SOL_SOCKET, SO_REUSEADDR, (char *)&tmp, sizeof(tmp));
            if (ret < 0)
            {
                fprintf(stdout, "%s:%d setsockopt failed: errno %d %s\n", __FILE__, __LINE__, errno, strerror(errno));
            }

            ret = ::bind(stream, (struct sockaddr *)&addr.sin6, sizeof(struct sockaddr_in6));
            if (ret < 0)
            {
                fprintf(stdout, "%s:%d bin failed: errno %d %s\n", __FILE__, __LINE__, errno, strerror(errno));
                close(stream);
                stream = -1;
                break;
            }

            break;
        }
        else
        {
            // ????????????????
            continue;
        }

        // ????????????????????
        // inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);

        // ??????
        // printf("%s: %s\n", ipver, ipstr);
    }

    // ??????
    freeaddrinfo(result);

    if (stream > 0)
    {
        if (::listen(stream, 24) != 0)
        {
            fprintf(stdout, "%s:%d socket_listen failed: errno %d\n", __FILE__, __LINE__, errno);
            close(stream);
            return Err(std::string(StrError(Errno)));
        }
    }

    if (stream < 0)
        return Err(std::string(StrError(Errno)));
    else
        return Ok(TcpListener(stream));
}

TcpListener::TcpListener(int fd) : TcpStream(fd)
{
}

TcpListener::TcpListener(TcpListener &&other) : TcpStream(other.fd)
{
    other.fd  = -1;
}

Result<TcpStream> TcpListener::Accept()
{
    struct sockaddr_in6 clent_addr;
    int addr_size = sizeof(struct sockaddr_in6);

    int connect_fd = ::accept(this->fd, (struct sockaddr *)&clent_addr, (socklen_t *)&addr_size);
    if (connect_fd < 0)
    {
        fprintf(stdout, "%s:%d accept failed: [errno %d] %s\n", __FILE__, __LINE__, errno, strerror(errno));
        return Err(std::string(StrError(Errno)));
    }

    return Ok(TcpStream(connect_fd));
}

Result<TcpStream> TcpListener::AccepT_timeout(uint32_t msecond)
{
    /* 如果有超时时间，调用select判断在超时时间内，是否有数据传输进来 */
    struct timeval timeout;
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = msecond / 1000;
    timeout.tv_usec = (msecond % 1000) * 1000;

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(this->fd, &fdset);

    do
    {
        int ret = ::select(this->fd + 1, &fdset, NULL, NULL, &timeout);
        /* 在指定时间内，若检测到??口可读，先不做???理，等待后??使用accept接收数据，否则直接返?? */
        if (ret > 0)
        {
            if (FD_ISSET(this->fd, &fdset))
            {
                ;
            }
            else
            {
                return Err(std::string(StrError(Errno)));
            }
        }
        else if (0 == ret) // select超时
        {
            // NETBASE_ERR("time out.\n");
            return Err(std::string(StrError(Errno)));
        }
        else if (errno == EINTR)
        {
            continue; // goto retry;
        }
        else // select错???
        {
            // NETBASE_ERR( "select err.\n");
            return Err(std::string(StrError(Errno)));
        }
    } while (0);

    struct sockaddr_in sin4;  /**< IPV4 地址*/
    struct sockaddr_in6 sin6; /**< IPV6 地址*/
    int addr_size = sizeof(struct sockaddr_in);
    int connect_fd = accept(this->fd, (struct sockaddr *)&(sin4), (socklen_t *)&addr_size);
    if (connect_fd < 0)
    {
        // NETBASE_ERR("socket_accept_with_timeout failed: errno %d\n", errno);
        return Err(std::string(StrError(Errno)));
    }

    return Ok(TcpStream(connect_fd));
}
