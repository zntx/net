#include <iostream>
#include "src/TcpStream.h"
int test_client(int agrc, char *agrv[])
{
    Result<TcpStream, int> stream_pack = TcpStream::Connect("127.0.0.1:9005");
    if (stream_pack.isErr())
    {
        printf("connect fail %d \n", stream_pack.unwrapErr());
        return -1;
    }
    TcpStream stream = stream_pack.unwrap();

    char data[] = "abcdefg";
    Slice<uint8_t> slice((uint8_t *)&data[0], sizeof(data));
    stream.write(slice);

    std::size_t len = stream.read(slice);

    printf("read size %lu\n", len);
    printf("read  %s\n", slice.addr);
}

int test_server(int agrc, char *agrv[])
{
    Result<TcpListener, int> listener_pack = TcpListener::bin("0.0.0.0", 9005);
    if (listener_pack.isErr())
    {
        printf("connect fail %d \n", listener_pack.unwrapErr());
        return 1;
    }
    TcpListener listener = listener_pack.unwrap();

    printf(" listener %d\n", listener.fd);

    Result<TcpStream, int> stream_pack = listener.Accept();
    if (stream_pack.isErr())
    {
        printf("Accept fail %d \n", stream_pack.unwrapErr());
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
   test_client(agrc, agrv);
}