
#include <chrono>
#include <string>
#include <stdint.h>
#include "buff.h"
#include "Result.h"
#include "SocketAddr.h"


using namespace std;
using namespace std::chrono;

class TcpStream
{

    int fd;

public:
    static Result<TcpStream,int> connect(std::string domain);
    static Result<TcpStream,int> connect (const char* host, size_t port);
    static Result<TcpStream,int> connect_timeout(SocketAddr &addr, std::chrono::duration<int, std::ratio<1,2>> timeout);

    size_t write(Slice &slice);
    size_t read(Slice &slice);
    /* Returns the socket address of the remote peer of this TCP connection. */
    Result<SocketAddr,int> peer_addr();
    Result<SocketAddr,int> local_addr();

    //Result<void, string> shutdown(Shutdown how);
    Result<TcpStream,int> try_clone();
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



class TcpListener : TcpStream
{

public:
    std::pair<TcpStream, SocketAddr> accept();

    static TcpStream bind(std::string addr);

};
