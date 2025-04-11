#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

#include "Packet.h"

int main(int argc, char * argv[])
{

    //create server socket
    int ServerSocket;
    ServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ServerSocket == -1) {
        return -1;
    }

    //binds socket to address
    sockaddr_in SvrAddr;
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_addr.s_addr = INADDR_ANY;
    SvrAddr.sin_port = htons(27000);
    if (bind(ServerSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == -1){
        close(ServerSocket);
        return -1;
    }

    sockaddr_in CltAddr;					//Client Address for sending responses
    //int len = sizeof(struct sockaddr_in);	//Length parameter for the recvfrom function call
    socklen_t addr_len;
    // doing this on my mac so the recvfrom function needs a socklen_t and the int didnt work

    //TODO:  Add the UDP receive here
    char RxBuffer[1024] = {};
    recvfrom(ServerSocket, RxBuffer, sizeof(RxBuffer), 0, (struct sockaddr*)&CltAddr, &addr_len);

    PktDef packet(RxBuffer);

    packet.Display(std::cout);

    //recvfrom(ServerSocket, RxBuffer, sizeof(RxBuffer), 0, (struct sockaddr*)&CltAddr, &addr_len);

    //std::cout << RxBuffer << std::endl;
    close(ServerSocket);	    //closes server socket
    // close the binded server socket

    return 1;
}