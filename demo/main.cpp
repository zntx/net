#include <iostream>
#include "net.h"
int test_client(int agrc, char *agrv[])
{
    auto stream_pack = TcpStream::Connect("127.0.0.1:9005");
    if (stream_pack.is_err())
    {
        printf("connect fail %s \n", stream_pack.unwrap_err().c_str());
        return -1;
    }

    printf("connect OK \n");
    TcpStream stream = stream_pack.unwrap();

    char data[] = "abcdefg";
    Slice<uint8_t> slice((uint8_t *)&data[0], sizeof(data));
    stream.write(slice);

    std::size_t len = stream.read(slice);

    printf("read size %lu\n", len);
    printf("read  %s\n", slice.addr);

    return 0;
}

int test_server(int agrc, char *agrv[])
{
    auto listener_pack = TcpListener::bin("0.0.0.0", 9005);
    if (listener_pack.is_err())
    {
        printf("connect fail %s \n", listener_pack.unwrap_err().c_str());
        return 1;
    }
    TcpListener listener = listener_pack.unwrap();

    printf(" listener %d\n", listener.GetSocket());

    auto stream_pack = listener.Accept();
    if (stream_pack.is_err())
    {
        printf("Accept fail %d \n", stream_pack.unwrap_err().c_str());
        return 1;
    }

    TcpStream stream = stream_pack.unwrap();

    char data[] = "abcdefg";
    Slice<uint8_t> slice((uint8_t *)&data[0], sizeof(data));

    std::size_t len = stream.read(slice);

    printf("read size %lu\n", len);
    printf("read  %s\n", slice.addr);

    stream.write(slice);

    return 1;
}



int main(int agrc, char *agrv[])
{
//    auto ips = IpAddr::Create(std::string("192.168.1.1"));
//    if( ips.is_err())
//    {
//        printf("Create fail %s \n", ips.unwrap_err().c_str());
//    }
//
//    auto ips1 = IpAddr::Create(std::string("192.256.1.1"));
//    if( ips1.is_err())
//    {
//        printf("1 Create fail %s \n", ips1.unwrap_err().c_str());
//    }
//
//    auto ips2 = IpAddr::Create(std::string("192.1.1"));
//    if( ips2.is_err())
//    {
//        printf("ips2 Create fail %s \n", ips2.unwrap_err().c_str());
//    }
//
//    auto ips3 = IpAddr::Create(std::string("192.01.1.1"));
//    if( ips3.is_err())
//    {
//        printf("ips3 Create fail %s \n", ips3.unwrap_err().c_str());
//    }
//
//
//    auto ips4 = IpAddr::Create(std::string("192.01.1.1aaa"));
//    if( ips4.is_err())
//    {
//        printf("ips4 Create fail %s \n", ips4.unwrap_err().c_str());
//    }
//
//    auto ips5 = IpAddr::Create(std::string("www.baidu.com.cn"));
//    if( ips5.is_err())
//    {
//        printf("ips5 Create fail %s \n", ips5.unwrap_err().c_str());
//    }
//
//    auto ip6 = IpAddr::Create(std::string("2001:0db8:3c4g:0015:0000:0000:1a2f:1a2b"));
//    if( ip6.is_err())
//    {
//        printf("ip6 Create fail %s \n", ip6.unwrap_err().c_str());
//    }
//
//    auto ip7 = IpAddr::Create(std::string("2031:0000:1F11F:0000:0000:0100:11A0:ADDF"));
//    if( ip7.is_err())
//    {
//        printf("ip7 Create fail %s \n", ip7.unwrap_err().c_str());
//    }
//
//    auto ip8 = IpAddr::Create(std::string("1080:0:0:0:8:800:200C:417A1"));
//    if( ip8.is_err())
//    {
//        printf("ip8 Create fail %s \n", ip8.unwrap_err().c_str());
//    }
//
//    auto ip9 = IpAddr::Create(std::string("FF01:0:::101"));
//    if( ip9.is_err())
//    {
//        printf("ip9 Create fail %s \n", ip9.unwrap_err().c_str());
//    }
//
//    auto ip10 = IpAddr::Create(std::string("0:0:0:0:0:0:0::1"));
//    if( ip10.is_err())
//    {
//        printf("ip10 Create fail %s \n", ip10.unwrap_err().c_str());
//    }
//
//    auto ip11 = IpAddr::Create(std::string("0:0:0:0:0:0:0:01234"));
//    if( ip11.is_err())
//    {
//        printf("ip11 Create fail %s \n", ip11.unwrap_err().c_str());
//    }

    auto addr1 = SocketAddr::Create(std::string("10.112.219.204"));
    if( addr1.is_ok())
    {
        printf("addr1 Create OK %s \n", addr1.unwrap().to_string().c_str());
    }else
    {
       printf("addr1 Create fail %s \n", addr1.unwrap_err().c_str());
    }

    auto addr2 = SocketAddr::Create(std::string("10.112.219.204:80"));
    if( addr2.is_ok())
    {
        printf("addr2 Create OK %s \n", addr2.unwrap().to_string().c_str());
    }else
    {
        printf("addr2 Create fail %s \n", addr2.unwrap_err().c_str());
    }


    test_client(agrc, agrv);
}