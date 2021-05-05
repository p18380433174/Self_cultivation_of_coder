#include "tcpServer.hpp"

#define TCP_SERVER_LISTEN_NUM   10

TcpServer::TcpServer(std::function<void(int, string)> onError) : BaseSocket(onError, TCP)
{
    int opt = 1;

    // allow rebind socket when socket in time_wait status
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int));
}

void TcpServer::bind(const char *address, unsigned short port, std::function<void(int, string)> onError)
{
    if (inet_pton(AF_INET, address, &this->address.sin_addr) < 0)
    {
        onError(errno, "Invalid address. Address type not support.");
        return;
    }

    this->address.sin_family = AF_INET;
    this->address.sin_port = htons(port);

    if (::bind(this->sock, (struct sockaddr *)&(this->address), sizeof(sockaddr_in)) < 0)
    {
        onError(errno, "cannot bind the socket.");
        return;
    }
}

void TcpServer::bind(int port, std::function<void(int, string)> onError)
{
    bind("0.0.0.0", port, onError);
}

void TcpServer::listen(std::function<void(int, string)> onError)
{
    if (::listen(sock, TCP_SERVER_LISTEN_NUM) < 0)
    {
        onError(errno, "Error: server cannot listen the socket.");
        return;
    }

    accept(this, onError);
}

void TcpServer::accept(TcpServer *server, std::function<void(int, string)> onError)
{
    sockaddr_in newSocketInfo{};
    socklen_t newSocketInfoLength = sizeof(newSocketInfo);

    int newSock;
    while(!server->isClose)
    {
        newSock = ::accept(server->sock, (sockaddr *)&newSocketInfo, &newSocketInfoLength);
        if (errno == EBADE || errno == EINVAL)
        {
            onError(errno, "Error while accepting a new connection.");
            continue;
        }
        if (server->isClose || newSock < 0)
        {
            return;
        }
        TcpSocket newSocket([](int e, string err){
            EVENT_NO_USE(e);
            EVENT_NO_USE((err));
        }, newSock);

        newSocket.setAddress(newSocketInfo);

        // set onMessageRecv event and onSocketClose event
        server->onConnect(&newSocket);
        newSocket.listen();
    }
}

void TcpServer::close()
{
    shutdown(sock, SHUT_RDWR);
    BaseSocket::close();
}
