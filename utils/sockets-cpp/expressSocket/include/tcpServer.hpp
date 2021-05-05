#ifndef __SOCKETS_CPP_TCP_SERVER_HPP__
#define __SOCKETS_CPP_TCP_SERVER_HPP__

#include "baseSocket.hpp"
#include "tcpSocket.hpp"

class TcpServer : public BaseSocket
{
public:
    // event listeners:
    std::function<void(TcpSocket *)> onConnect = [](TcpSocket* sock){EVENT_NO_USE(sock)};

    explicit TcpServer(EVENT_ON_ERROR);

    // binding the server
    void bind(int port, EVENT_ON_ERROR);
    void bind(const char* address, unsigned short port, EVENT_ON_ERROR);

    // start listening the server
    void listen(EVENT_ON_ERROR);
    void close() override;

private:
    static void accept(TcpServer* server, EVENT_ON_ERROR);
};

#endif //__SOCKETS_CPP_TCP_SERVER_HPP__
