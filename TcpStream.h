
#include <chrono>
#include <string>
#include <stdint.h>
#include "slice/Slice.h"
#include "Result.h"
#include "SocketAddr.h"


using namespace std;
using namespace std::chrono;

class TcpStream
{

    int fd;

public:
    static Result<TcpStream> connect(std::string domain);
    static Result<TcpStream> connect (const char* host, size_t port);
    static Result<TcpStream> connect_timeout(SocketAddr &addr, std::chrono::duration<int, std::ratio<1,2>> timeout);

    size_t write(Slice &slice);
    size_t read(Slice &slice);
    /* Returns the socket address of the remote peer of this TCP connection. */
    Result<SocketAddr> peer_addr();
    Result<SocketAddr> local_addr();

    //Result<void, string> shutdown(Shutdown how);
    Result<TcpStream> try_clone();
   // Result<void, string> set_read_timeout(struct timeval *tv);
    //Result<void, string> set_read_timeout(Option<duration> dur);
    //Result<void, string> set_write_timeout(struct timeval *tv);
   // Result<void, string> set_write_timeout(Option<duration> dur);
    Result<Option<struct timeval>> read_timeout();
    Result<Option<struct timeval>> write_timeout();
    //Result<Option<duration>, Error> read_timeout();
    //Result<Option<duration>, Error> write_timeout();
    //Result<void, string> set_linger(linger : Option<duration>);
    //Result<Option<duration>, Err> linger();
   // Result<void, string> set_nodelay(bool nodelay  );
    Result<bool> nodelay();
    Result<void> set_ttl(uint32_t ttl  );
    Result<uint32_t> ttl();
    Result<void> take_error();
    Result<void> set_nonblocking(bool nonblocking);
};



class TcpListener : TcpStream
{

public:
    std::pair<TcpStream, SocketAddr> accept();

    static TcpStream bind(std::string addr);

};
