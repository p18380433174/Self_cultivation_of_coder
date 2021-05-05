#include "baseSocket.hpp"
#include <cerrno>

using std::function;

string BaseSocket::ipToString(sockaddr_in addr)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);

    return std::string(ip);
}

BaseSocket::BaseSocket(std::function<void(int, string)> onError, SocketType type, int socketId) : sock(socketId)
{
    if (socketId >= 0)
    {
        return;
    }
    sock = socket(AF_INET, type, 0);
    if (sock < 0)
    {
        onError(errno, "create socket error.");
    }
}

void BaseSocket::close()
{
    if (isClose)
    {
        return;
    }
    isClose = true;
    ::close(sock);
}

string BaseSocket::selfAddress()
{
    return ipToString(address);
}

int BaseSocket::selfPort()
{
    return ntohs(address.sin_port);
}