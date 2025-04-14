#include "MySocket.h"

MySocket::MySocket(SocketType socketType, const std::string& ip, unsigned int port, ConnectionType connectionType, unsigned int bufferSize = DEFAULT_SIZE)
        : mySocket(socketType), IPAddr(ip), Port(port), connectionType(connectionType), bTCPConnection(false) {

    if (bufferSize <= 0)
        MaxSize = DEFAULT_SIZE;
    else
        MaxSize = bufferSize;

    Buffer = new char[MaxSize];
    memset(Buffer, 0, MaxSize);

    if (connectionType == TCP)
        ConnectionSocket = socket(AF_INET, SOCK_STREAM, 0);
    else
        ConnectionSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (ConnectionSocket < 0) {
        std::cout << "Error creating socket" << std::endl;
        return;
    }

    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(port);
    SvrAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (mySocket == SERVER) {
        if (connectionType == TCP) {
            WelcomeSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (bind(WelcomeSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr)) == -1) {
                close(WelcomeSocket);
                std::cout << "Error binding TCP welcome socket" << std::endl;
                return;
            }
            if (listen(WelcomeSocket, 1) == -1) {
                close(WelcomeSocket);
                std::cout << "Error listening for welcome socket" << std::endl;
            }
        } else {
            if (bind(ConnectionSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr)) == -1) {
                close(ConnectionSocket);
                std::cout << "Error binding connection socket" << std::endl;
                return;
            }
        }
    }
}

MySocket::~MySocket() {
    if (Buffer)
        delete[] Buffer;
    if (bTCPConnection)
        close(ConnectionSocket);
    if(mySocket == SERVER && connectionType == TCP)
        close(WelcomeSocket);
}

void MySocket::ConnectTCP() {

    if (connectionType != TCP) {
        std::cout << "Error cant connect to TCP with UDP" << std::endl;
        return;
    }

    if (mySocket == CLIENT) {

        if (connect(ConnectionSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr)) < 0)
            std::cout << "Error TCP connection failed" << std::endl;
        else
            bTCPConnection = true;

    } else if (mySocket == SERVER) {

        sockaddr_in clientAddr {};
        socklen_t clientLen = sizeof(clientAddr);
        ConnectionSocket = accept(WelcomeSocket, (sockaddr*)&clientAddr, &clientLen);

        if (ConnectionSocket >= 0)
            bTCPConnection = true;
        else
            std::cout << "Error server didnt accept TCP client" << std::endl;
    }
}

void MySocket::DisconnectTCP() {
    if (connectionType != TCP || !bTCPConnection)
        return;

    close(ConnectionSocket);
    bTCPConnection = false;
}

void MySocket::SendData(const char* buff, int size) {
    if (connectionType == TCP)
        send(ConnectionSocket, buff, size, 0);
    else
        sendto(ConnectionSocket, buff, size, 0, (sockaddr*)&SvrAddr, sizeof(SvrAddr));
}

int MySocket::GetData(char* buff) {
    memset(Buffer, 0, MaxSize);

    int bytes = 0;

    if(connectionType == TCP) {
        bytes = recv(ConnectionSocket, Buffer, MaxSize, 0);
    } else {
         socklen_t len = sizeof(SvrAddr);
         bytes = recvfrom(ConnectionSocket, Buffer, MaxSize, 0, (sockaddr*)&SvrAddr, &len);
    }
    memcpy(buff, Buffer, bytes);

    return bytes;
}

std::string MySocket::GetIPAddr() {
    return IPAddr;
}

void MySocket::SetIPAddr(const std::string& ip) {
    if (bTCPConnection) {
        std::cout << "Error trying to change ip while connected" << std::endl;
        return;
    }
    IPAddr = ip;
    SvrAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str());
}

int MySocket::GetPort() {
    return Port;
}

void MySocket::SetPort(int port) {
    if (bTCPConnection) {
        std::cout << "Error trying to change port number while connected" << std::endl;
        return;
    }

    Port = port;
    SvrAddr.sin_port = htons(Port);
}

SocketType MySocket::GetType() {
    return mySocket;
}

void MySocket::SetType(SocketType socketType) {
    if (bTCPConnection || (mySocket == SERVER && connectionType == TCP && WelcomeSocket > 0)) {
        std::cout << "Error trying to change port number while connected" << std::endl;
        return;
    }
    mySocket = socketType;
}