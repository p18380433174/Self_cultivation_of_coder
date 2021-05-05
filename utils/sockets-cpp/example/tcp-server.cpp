#include "tcpServer.hpp"
#include <iostream>

using namespace std;

#define TCP_SERVER_TRACE(log)   cout << (log) << endl
#define TCP_SERVER_PORT         8888
#define TCP_SOCKET_INFO(sock)   string("[" + newClient->selfAddress() + ":" + to_string(newClient->selfPort()) + "]")
#define ERROR_HANDLER   [](int errorCode, string errorMessage){\
    TCP_SERVER_TRACE(errorMessage + ", error code: " + errorCode);\
}

string operator+(const string& left, int number)
{
    return left + to_string(number);
}

int main(int argc, char* argv[])
{
    // initialize server socket
    TcpServer tcpServer;

    // set connect event
    tcpServer.onConnect = [](TcpSocket* newClient) {
        TCP_SERVER_TRACE("New client" + TCP_SOCKET_INFO(newClient));
        newClient->onMessageReceive = [newClient](string message) {
            TCP_SERVER_TRACE(TCP_SOCKET_INFO(newClient) + " => " + TCP_SOCKET_INFO(tcpServer) + ":" + message);
            // exit when finish command handle
            newClient->isTaskFinish = true;
        };

        newClient->onSocketClosed = [newClient](int errorCode) {
            TCP_SERVER_TRACE(TCP_SOCKET_INFO(newClient) + " closed.");
        };
    };

    // bind port to server
    tcpServer.bind(TCP_SERVER_PORT, ERROR_HANDLER);

    // start listen client connect
    tcpServer.listen(ERROR_HANDLER);
}