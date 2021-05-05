#ifndef __SOCKETS_CPP_BASE_SOCKET_HPP__
#define __SOCKETS_CPP_BASE_SOCKET_HPP__

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <functional>

using std::string;
#define EVENT_NO_USE(expr)  {(void)(expr);}
#define EVENT_ON_ERROR  std::function<void(int, string)> onError = \
    [](int errorCode, string errorMessage){EVENT_NO_USE(errorCode); EVENT_NO_USE(errorMessage)}
class BaseSocket
{
protected:
    int sock = 0;
    static string ipToString(sockaddr_in addr);

public:
    const unsigned short BUFFER_SIZE = 0xffff;
    enum SocketType
    {
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };

    sockaddr_in address;
    bool isClose = false;

    explicit BaseSocket(EVENT_ON_ERROR, SocketType type = TCP, int socketId = -1);

    virtual void close();

    string selfAddress();

    int selfPort();
};


#endif //__SOCKETS_CPP_BASE_SOCKET_HPP__
