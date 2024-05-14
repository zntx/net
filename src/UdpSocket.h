#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include <chrono>
#include <string>
#include <stdint.h>
#include "Slice.h"
#include "Result.h"
#include "SocketAddr.h"
#include "Socket.h"

using namespace std;
using namespace std::chrono;

class UdpSocket : public Socket
{
public:
    static Result<UdpSocket,int> Bind(std::string domain);
    static Result<UdpSocket,int> Connect(std::string domain);
    static Result<UdpSocket,int> Connect (const char* host, size_t port);
    static Result<UdpSocket,int> Connect_timeout(SocketAddr &addr, std::chrono::duration<int, std::ratio<1,2>> timeout);

    UdpSocket(int fd);
    size_t write(Slice<uint8_t> slice);
    size_t read(Slice<uint8_t> slice);
    /* Returns the socket address of the remote peer of this TCP connection. */
    Result<SocketAddr,int> peer_addr();
    Result<SocketAddr,int> local_addr();

    //Result<void, string> shutdown(Shutdown how);
    Result<UdpSocket,int> try_clone();
   // Result<void, string> set_read_timeout(struct timeval *tv);
    //Result<void, string> set_read_timeout(Option<duration> dur);
    //Result<void, string> set_write_timeout(struct timeval *tv);
   // Result<void, string> set_write_timeout(Option<duration> dur);
    Result<Option<struct timeval>,int> read_timeout();
    Result<Option<struct timeval>,int> write_timeout();
    //Result<Option<duration>, Error> read_timeout();
    //Result<Option<duration>, Error> write_timeout();
    //Result<void, string> set_linger(linger : Option<duration>);
    //Result<Option<duration>, Err> linger();
   // Result<void, string> set_nodelay(bool nodelay  );
    Result<bool,int> nodelay();
    Result<void,int> set_ttl(uint32_t ttl  );
    Result<uint32_t, int> ttl();
    Result<void,int> take_error();
    Result<void,int> set_nonblocking(bool nonblocking);
};

#endif