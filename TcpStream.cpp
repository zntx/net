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
#include "TcpStream.h"


Result<TcpStream> TcpStream::connect(std::string domain)
{
	std::size_t pos = domain.find(":"); // position of "live" in str
	if( pos > 0) {
        std::string host = domain.substr (0, pos); // get from "live" to the end
        std::string ports = domain.substr (pos+1); // get from "live" to the end
        //std::cout << host << ' ' << ports << '\n';
        int port = atoi(ports.c_str());
        return TcpStream::connect(host.c_str(), port );
    }
    else {
        return TcpStream::connect(domain.c_str(), 80 );
    }
}


Result<TcpStream> TcpStream::connect (const char* host, size_t port)
{

    TcpStream strem;
    ///定义sockfd
    strem.fd = socket(AF_INET, SOCK_STREAM, 0);
    
    ///定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(port);
    server_sockaddr.sin_addr.s_addr = inet_addr(host);
    
    ///bind，成功返回0，出错返回-1
    if(bind(  strem.fd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) ==-1)
    {
        //perror("bind");
        //string str(strerror(errno));
        //return Err( str);
    }
    
    ///listen，成功返回0，出错返回-1
    if(listen(strem.fd, 24) == -1)
    {
        perror("listen");
        exit(1);
    }

    return Ok(strem);
}

Result<TcpStream> connect_timeout( SocketAddr &addr , std::chrono::duration<int, std::ratio<1,2>>  timeout )
{

}



size_t TcpStream::read( Slice &slice) 
{
    return recv(this->fd, slice.address, slice.len, 0);
}

size_t TcpStream::write( Slice &slice) 
{
    return send(this->fd, slice.address, slice.len, 0);
}


Result<SocketAddr> TcpStream::peer_addr()
{
    struct sockaddr_in peerAddr;
    
    uint32_t peerLen = sizeof(peerAddr);
    if(getpeername(this->fd, (struct sockaddr *)&peerAddr, &peerLen) == -1){
        printf("getpeername error\n");
        //string str = strerror(errno);
        //return Err(str);
    }
    
    SocketAddr addr_ = SocketAddr(SocketAddrV4(peerAddr));
    return Ok(addr_);
}

Result<SocketAddr> TcpStream::local_addr()
{
    struct sockaddr_in connectedAddr;
    uint32_t len = sizeof(connectedAddr);
    if(getsockname( this->fd, (struct sockaddr *)&connectedAddr, &len) == -1){
        printf("getsockname error\n");
        //string str = strerror(errno);
        //return Err(str);
    }
    
    //SocketAddr addr(SocketAddrV4(connectedAddr));
    SocketAddr addr_ = SocketAddr(SocketAddrV4(connectedAddr));
    return Ok( addr_);
}


// Result<void, string> TcpStream::set_read_timeout(struct timeval *tv)
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

// Result<void, string> TcpStream::set_write_timeout(struct timeval *tv)
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

// int flags = fcntl(fd, F_GETFL);
//     flags |= O_NONBLOCK;
//     ret = fcntl(fd, F_SETFL, flags);

Result<Option<struct timeval>> TcpStream::read_timeout()
{

    struct timeval tv;
    tv.tv_sec = 10; tv.tv_usec = 0;
    socklen_t optlen = sizeof(struct timeval);
    //getsockopt(this->fd, SOL_SOCKET,SO_RCVTIMEO, &tv, &optlen); 
    setsockopt(this->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, optlen);

    Option<struct timeval> ret = Some(tv);
    return Ok(ret);
}
Result<Option<struct timeval>> TcpStream::write_timeout()
{
    struct timeval tv;
    tv.tv_sec = 10; tv.tv_usec = 0;
    socklen_t optlen = sizeof(struct timeval);
    //getsockopt(this->fd, SOL_SOCKET,SO_RCVTIMEO, &tv, &optlen); 
    setsockopt(this->fd, SOL_SOCKET, SO_SNDTIMEO, &tv, optlen);

    Option<struct timeval> ret = Some(tv);
    return Ok(ret);
}

// Result<void, string> TcpStream::set_nodelay(bool _nodelay  )
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

Result<bool> TcpStream::nodelay()
{
    int nodelay = 0;
	int rc = setsockopt(this->fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(int));

    if( nodelay)
        return Ok(true);
    else 
        return Ok(false);
}


// Result<void, string> TcpStream::set_nonblocking(bool nonblocking)
// {
//     int block_or_not = 1; // 设置非阻塞
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

// Result<void, string> TcpStream::set_ttl(uint32_t ttl  )
// {

//     return Ok();
// }
Result<uint32_t> TcpStream::ttl()
{
    uint32_t x =3;
    return Ok(x);
}


// Result<TcpStream, string> TcpStream::connect(std::string domain)
// {   
//     TcpStream strem;

//     return Ok(strem);
// }
