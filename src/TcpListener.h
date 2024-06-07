//
// Created by zhangyuzhu8 on 2024/6/7.
//

#ifndef NET_TCPLISTENER_H
#define NET_TCPLISTENER_H

#include <chrono>

#include <string>
#include <stdint.h>
#include "Slice.h"
#include "Result.h"
#include "SocketAddr.h"
#include "Socket.h"
#include "TcpStream.h"

using namespace std;
using namespace std::chrono;

class TcpListener : public Socket
{

public:
    static Result<TcpListener> Bin(SocketAddr host , int backlog =24);
    static Result<TcpListener> Bin(Slice<const char> host, uint16_t port = 80);
    static Result<TcpListener> Bin(std::string domain, uint16_t port = 80);

    TcpListener(SOCKET fd);
    TcpListener(TcpListener &other) = delete;
    TcpListener(const TcpListener &other) = delete;
    TcpListener(TcpListener &&other);


    Result<std::pair<TcpStream, SocketAddr>> accept();

    Result<std::pair<TcpStream, SocketAddr>> accept_timeout(uint32_t msecond);

};


#endif //NET_TCPLISTENER_H
