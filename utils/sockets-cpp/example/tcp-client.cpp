#include "tcpSocket.hpp"
#include <iostream>

using namespace std;

#define TCP_SERVER_PORT 8888

int main(int argc, char* argv[])
{
    string cmd;
    for (int i = 0; i < argc; i++)
    {
        cmd += string(argv[i]) + " ";
    }
    cmd.pop_back();
    cout << "input cmd is: " << cmd << ", cmd length is:" << cmd.length() << endl;

    TcpSocket tcpSocket;
    tcpSocket.onMessageReceive = [](string message) {
        cout << "Message from server: " + message << endl;
    };

    tcpSocket.onSocketClosed = [](int errorCode) {
        cout << "Connect closed. exit code:" << errorCode << endl;
    };

    tcpSocket.connect("localhost", TCP_SERVER_PORT, [&]{
        cout << "Connect to the server successfully." << endl;

        tcpSocket.send(cmd);
    },[](int errorCode, string errorMessage){
        cout << errorMessage << ", error code:" << errorCode << endl;
    });
}



