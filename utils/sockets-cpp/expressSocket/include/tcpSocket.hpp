#ifndef __SOCKETS_CPP_TCP_SOCKET_HPP__
#define __SOCKETS_CPP_TCP_SOCKET_HPP__

#include "baseSocket.hpp"

#define EVENT_ON_CONNECT std::function<void()> onConnect = [](){}
class TcpSocket : public BaseSocket
{
public:
    // event listeners:
    std::function<void(string)> onMessageReceive;
    std::function<void(const char*, int)> onRawMessageReceive;
    std::function<void(int)> onSocketClosed;
    thread_local static bool isTaskFinish;

    explicit TcpSocket(EVENT_ON_ERROR, int socketId = -1);

    size_t send(const string& message);
    size_t send(const char* bytes, size_t bytesLength);

    void connect(const string& host, unsigned short  port, EVENT_ON_CONNECT, EVENT_ON_ERROR);
    void connect(unsigned int ipv4, unsigned short port, EVENT_ON_CONNECT, EVENT_ON_ERROR);

    void listen();

    void setAddress(sockaddr_in addr);
    sockaddr_in getAddress() const;
private:
    static void receive(TcpSocket* socket);

    void setTimeOut(int seconds);
};

#endif
