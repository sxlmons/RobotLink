
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include "Packet.h"

using namespace std;

int main(int argc, char* argv[])
{

    //initializes socket. SOCK_STREAM: TCP
    int ClientSocket;
    ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ClientSocket == -1) {
        return -1;
    }

    //Connect socket to specified server
    sockaddr_in SvrAddr;
    SvrAddr.sin_family = AF_INET;						//Address family type itnernet
    SvrAddr.sin_port = htons(27000);					//port (host to network conversion)
    SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP address

    PktDef packet;
    packet.SetPktCount(1);
    packet.SetCmd(PktDef::DRIVE);

    packet.driverBody.Direction = 1;
    packet.driverBody.Duration = 10;
    packet.driverBody.Speed = 80;

    packet.SetBodyData(reinterpret_cast<char*>(&packet.driverBody), sizeof(packet.driverBody));

    char* packetBuffer = packet.GenPacket();

    PktDef newPacket(packetBuffer);

    if (newPacket.CheckCRC(packetBuffer, newPacket.GetLength()))
        cout << "TRUE" << endl;
    else cout << "FALSE" << endl;

    newPacket.Display(std::cout);
    sendto(ClientSocket, packetBuffer, packet.GetLength(), 0, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));

    //sendto(ClientSocket, Tx, Size, 0, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr));

    close(ClientSocket);

    return 1;
}