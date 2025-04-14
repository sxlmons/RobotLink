#ifndef WEEK_2_CODE_MYSOCKET_H
#define WEEK_2_CODE_MYSOCKET_H

#include <iostream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

// FOR DOCKER TO VM 
#include <netdb.h>

enum SocketType {
    CLIENT,
    SERVER
};

enum ConnectionType {
    TCP,
    UDP
};

#define DEFAULT_SIZE 1024 // defines the default size of the buffer space

class MySocket {
    char* Buffer;

    int WelcomeSocket;
    int ConnectionSocket;

    struct sockaddr_in SvrAddr;

    SocketType mySocket;

    std::string IPAddr;
    int Port;

    ConnectionType connectionType;

    bool bTCPConnection;

    unsigned int MaxSize;

public:

    MySocket(SocketType, const std::string&, unsigned int, ConnectionType, unsigned int);

    ~MySocket();

    void ConnectTCP();

    void DisconnectTCP();

    void SendData(const char*, int );

    int GetData(char*);

    std::string GetIPAddr();

    void SetIPAddr(const std::string&);

    int GetPort();

    void SetPort(int);

    SocketType GetType();

    void SetType(SocketType);
};

#endif //WEEK_2_CODE_MYSOCKET_H
