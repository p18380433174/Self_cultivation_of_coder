#include "tcpSocket.hpp"
#include <iostream>
#include <thread>
#include <cstring>

#define SOCKET_CPP_TRACE(log)           std::cout << (log) << std::endl;
#define TCP_RCV_SND_TIMEOUT_SECONDS     5

thread_local bool TcpSocket::isTaskFinish = false;

TcpSocket::TcpSocket(std::function<void(int, string)> onError, int socketId) :
        BaseSocket(onError, TCP, socketId)
{

}

size_t TcpSocket::send(const string& message)
{
    send(message.c_str(), message.length());
}

size_t TcpSocket::send(const char* bytes, size_t bytesLength)
{
    if (isClose)
    {
        return -1;
    }

    auto sent = ::send(sock, bytes, bytesLength, 0);
    if (sent < 0)
    {
        SOCKET_CPP_TRACE("tcp send error!");
    }

    return sent;
}

void TcpSocket::connect(const string& host, unsigned short port, std::function<void()> onConnect,
                        std::function<void(int, string)> onError)
{
    struct addrinfo hints{};
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = TCP;

    struct addrinfo* res = nullptr;
    int status = getaddrinfo(host.c_str(), nullptr, &hints, &res);
    if (status != 0)
    {
        onError(errno, "Invalid address." + string(gai_strerror(status)));
        return;
    }

    for (auto it = res; it != nullptr; it = it->ai_next)
    {
        if (it->ai_family != AF_INET)
        {
            continue;
        }
        memcpy((void *)&address, (void *)it->ai_addr, sizeof(sockaddr_in));
        break;
    }

    freeaddrinfo(res);

    connect((unsigned int)address.sin_addr.s_addr, port, onConnect, onError);
}

void TcpSocket::connect(unsigned int ipv4, unsigned short port, std::function<void()> onConnect,
                        std::function<void(int, string)> onError)
{
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = ipv4;

    setTimeOut(TCP_RCV_SND_TIMEOUT_SECONDS);

    // try to connect, if cannot connect to host in 5s will timeout
    if (::connect(sock, (const sockaddr *)&address, sizeof(sockaddr_in)) < 0)
    {
        onError(errno, "connect to the host failed!");
        return;
    }

    setTimeOut(0);

    // connect to server, fire the event
    onConnect();

    this->listen();
}

void TcpSocket::setTimeOut(int seconds)
{
    struct timeval tv{};
    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv));
}

void TcpSocket::receive(TcpSocket *socket)
{
    char tempBuffer[socket->BUFFER_SIZE];
    int messageLength;

    while ((messageLength = recv(socket->sock, tempBuffer, socket->BUFFER_SIZE, 0)) > 0)
    {
        tempBuffer[messageLength] = '\0';
        if (socket->onMessageReceive)
        {
            socket->onMessageReceive(string(tempBuffer).substr(0, messageLength));
        }

        if (socket->onRawMessageReceive)
        {
            socket->onRawMessageReceive(tempBuffer, messageLength);
        }

        if (isTaskFinish)
        {
            break;
        }
    }

    socket->close();
    if (socket->onSocketClosed)
    {
        socket->onSocketClosed(errno);
    }
}

void TcpSocket::listen()
{
    // start listening the socket from thread
    std::thread(TcpSocket::receive, this).join();
}

void TcpSocket::setAddress(sockaddr_in addr)
{
    address = addr;
}

sockaddr_in TcpSocket::getAddress() const
{
    return address;
}


