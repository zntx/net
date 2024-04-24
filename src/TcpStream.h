/*


1.0.0 · source · [−]
*/
#include <chrono>
#include <string>
#include <stdint.h>
#include "Slice.h"
#include "Result.h"
#include "SocketAddr.h"

using namespace std;
using namespace std::chrono;

class TcpStream
{

public:
    int fd;
    static Result<TcpStream, int> Connect(std::string domain);
    static Result<TcpStream, int> Connect(const char *host, size_t port);
    static Result<TcpStream, int> Connect_timeout(SocketAddr &addr, std::chrono::duration<int, std::ratio<1, 2>> timeout);

    ~TcpStream();
    TcpStream(int fd);
    TcpStream(TcpStream &&other);
    TcpStream(const TcpStream &other);
    TcpStream &operator=(TcpStream &&other);

    size_t write(Slice<uint8_t> &slice);
    size_t read(Slice<uint8_t> &slice);
    /* Returns the socket address of the remote peer of this TCP connection. */
    Result<SocketAddr, int> peer_addr();
    Result<SocketAddr, int> local_addr();

    // Result<void, string> shutdown(Shutdown how);
    Result<TcpStream, int> try_clone();
    // Result<void, string> set_read_timeout(struct timeval *tv);
    // Result<void, string> set_read_timeout(Option<duration> dur);
    // Result<void, string> set_write_timeout(struct timeval *tv);
    // Result<void, string> set_write_timeout(Option<duration> dur);
    Result<Option<struct timeval>, int> read_timeout();
    Result<Option<struct timeval>, int> write_timeout();
    // Result<Option<duration>, Error> read_timeout();
    // Result<Option<duration>, Error> write_timeout();
    // Result<void, string> set_linger(linger : Option<duration>);
    // Result<Option<duration>, Err> linger();
    // Result<void, string> set_nodelay(bool nodelay  );
    Result<bool, int> nodelay();
    Result<void, int> set_ttl(uint32_t ttl);
    Result<uint32_t, int> ttl();
    Result<void, int> take_error();
    Result<void, int> set_nonblocking(bool nonblocking);
};

class TcpListener : public TcpStream
{

public:
    static Result<TcpListener, int> bin(const char *host_name, uint16_t port);

    TcpListener(int fd);
    TcpListener(TcpListener &&other);
    Result<TcpStream, int> Accept();

    Result<TcpStream, int> AccepT_timeout(uint32_t msecond);

};
