/*


1.0.0 · source · [−]
*/
#include <chrono>
#include <string>
#include <stdint.h>
#include "Slice.h"
#include "Result.h"
#include "SocketAddr.h"
#include "Socket.h"

using namespace std;
using namespace std::chrono;

class TcpStream : public Socket
{

public:
    static Result<TcpStream> Connect(std::string domain);
    static Result<TcpStream> Connect(Slice<const char> host, size_t port);
    static Result<TcpStream> Connect(SocketAddr &addr);

    static Result<TcpStream, int> Connect_timeout(SocketAddr &addr, std::chrono::duration<int, std::ratio<1, 2>> timeout);

    static Result<TcpStream> Connect_timeout(std::string domain, struct timeval);
    static Result<TcpStream> Connect_timeout(Slice<const char> host, size_t port, struct timeval);
    static Result<TcpStream> Connect_timeout(SocketAddr &addr, struct timeval);

    ~TcpStream();
    TcpStream(int fd);
    TcpStream(TcpStream &&other);
    TcpStream(TcpStream &other) = delete;
    TcpStream(const TcpStream &other) = delete;
    TcpStream &operator=(TcpStream &&other);

    size_t write(Slice<uint8_t> &slice);
    size_t read(Slice<char> &slice);
    size_t read(Slice<uint8_t> &slice);
    /* Returns the socket address of the remote peer of this TCP connection. */
    //Result<SocketAddr, int> peer_addr();
    //Result<SocketAddr, int> local_addr();

    // Result<void, string> shutdown(Shutdown how);
    Result<TcpStream, int> try_clone();
    // Result<void, string> set_read_timeout(struct timeval *tv);
    // Result<void, string> set_read_timeout(Option<duration> dur);
    // Result<void, string> set_write_timeout(struct timeval *tv);
    // Result<void, string> set_write_timeout(Option<duration> dur);
    Result<Option<struct timeval>> read_timeout();
    Result<Option<struct timeval>> write_timeout();
    // Result<Option<duration>, Error> read_timeout();
    // Result<Option<duration>, Error> write_timeout();
    // Result<void, string> set_linger(linger : Option<duration>);
    // Result<Option<duration>, Err> linger();
    // Result<void, string> set_nodelay(bool nodelay  );
    //Result<bool> nodelay();
    Result<void, int> set_ttl(uint32_t ttl);
    Result<uint32_t> ttl();
    //Result<void, int> take_error();
    //Result<void, int> set_nonblocking(bool nonblocking);
};

class TcpListener : public TcpStream
{

public:
    static Result<TcpListener> bin(const char *host_name, uint16_t port);

    TcpListener(int fd);
    TcpListener(TcpListener &&other);
    Result<TcpStream> Accept();

    Result<TcpStream> AccepT_timeout(uint32_t msecond);

};
