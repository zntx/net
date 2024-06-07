#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <utility>
#include "UdpSocket.h"


Result<UdpSocket> UdpSocket::Connect(SocketAddr host, struct timeval timeout)
{
    int family = host.is_v4() ? AF_INET : AF_INET6;

    auto client = Socket::Create(family, SOCK_STREAM, IPPROTO_TCP);
    if( client.is_err())
    {
        printf("Socket::Create error\n");
        return Err(client.unwrap_err());
    }

    auto _client  = client.unwrap();

    /* 没有超时时间*/
    if( timeout.tv_sec == 0 && timeout.tv_usec == 0)
    {
        auto ret = _client.connect(host);

        if( ret.is_err())
        {
            return Err(ret.unwrap_err());
        }

        return Ok(UdpSocket(_client.take()));
    }

    // 清除错误
    int opt_val = 0;
    int length = sizeof(opt_val);
    auto socket_fd = _client.get_socket( );

    _client.set_nodelay(1);

    printf("time start %lld\n", time(nullptr));
    auto ret = _client.connect(host);
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

            if(::select(socket_fd + 1, nullptr, &set, nullptr, &timeout) > 0)
            {
                getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, (char*)&opt_val, (socklen_t*)&length);
            }
        }
    }
    printf("time end %lld\n", time(nullptr));
    _client.set_nodelay(0);

    if (0 == opt_val)
    {
        return Ok(UdpSocket(_client.take()));
    }
    else{
        return Err(client.unwrap_err());
    }
}

Result<UdpSocket> UdpSocket::Connect(Slice<const char> host, size_t port,  struct timeval timeout)
{
    auto stream = INVALID_SOCKET;
    struct addrinfo hints{0};
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags =  AI_ALL;       //AI_NUMERICHOST; //
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

            auto ip_addr =  SocketAddr((struct sockaddr_storage*)ai->ai_addr);

            ip_addr.set_port(port);

            std::cout << "ip_addr :" << ip_addr.to_string() << "." << std::endl;

            auto client = Connect(ip_addr, timeout);
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

        auto client = Connect(ips, timeout);
        if( client.is_err())
            return Err(_ips.unwrap_err());

        stream = client.unwrap().take();

    }

    if( stream == INVALID_SOCKET)
        return Err(std::string(StrError(Errno)));

    return Ok(UdpSocket(stream));
}

Result<UdpSocket> UdpSocket::Connect(const std::string& domain,  struct timeval timeout )
{
    std::size_t pos = domain.find(':'); // position of "live" in str
    if (pos > 0)
    {
        std::string host = domain.substr(0, pos);   // get from "live" to the end
        std::string ports = domain.substr(pos + 1); // get from "live" to the end
        // std::cout << host << ' ' << ports << '\n';
        int port = atoi(ports.c_str());
        return UdpSocket::Connect({host.c_str(), host.length()}, port, timeout);
    }
    else
    {
        return UdpSocket::Connect( {domain.c_str(), domain.length()}, 80, timeout);
    }
}

Result<UdpSocket> UdpSocket::Connect(SocketAddr host, uint64_t microseconds)
{
    struct timeval timeout{0,0};
    timeout.tv_sec = microseconds / 1000;
    timeout.tv_usec = (microseconds % 1000) * 1000;

    return Connect( std::move(host), timeout);
}

Result<UdpSocket> UdpSocket::Connect(Slice<const char> host, size_t port, uint32_t microseconds)
{
    struct timeval timeout{0, 0};
    timeout.tv_sec = microseconds / 1000;
    timeout.tv_usec = (microseconds % 1000) * 1000;

    return Connect( std::move(host), port, timeout);
}

Result<UdpSocket> UdpSocket::Connect(const std::string& domain, uint32_t microseconds )
{
    std::size_t pos = domain.find(':'); // position of "live" in str
    if (pos > 0)
    {
        std::string host = domain.substr(0, pos);   // get from "live" to the end
        std::string ports = domain.substr(pos + 1); // get from "live" to the end
        // std::cout << host << ' ' << ports << '\n';
        int port = atoi(ports.c_str());
        return UdpSocket::Connect({host.c_str(), host.length()}, port, microseconds);
    }
    else
    {
        return UdpSocket::Connect( {domain.c_str(), domain.length()}, 80, microseconds);
    }
}

UdpSocket::UdpSocket(SOCKET fd)
{
    this->fd = fd;
}
size_t UdpSocket::read(const Slice<uint8_t>& slice)
{
    return recv(this->fd, (char*)slice.addr, slice.len, 0);
}

size_t UdpSocket::write(const Slice<uint8_t>& slice)
{
    return send(this->fd, (char*)slice.addr, slice.len, 0);
}
