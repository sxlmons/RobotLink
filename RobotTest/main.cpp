#include "MySocket/MySocket.h"
#include "Packet/Packet.h"

using namespace std;

string printDriveDirection(int dir);

int main() {

    MySocket client(CLIENT, "127.0.0.1", 27000, UDP, 1024);

    PktDef packet;
    packet.SetPktCount(1);
    packet.SetCmd(PktDef::DRIVE);

    packet.driverBody.Direction = 1;
    packet.driverBody.Duration = 10;
    packet.driverBody.Speed = 80;
    packet.SetBodyData(reinterpret_cast<char*>(&packet.driverBody), sizeof(packet.driverBody));

    char* packetBuffer = packet.GenPacket();

    client.SendData(packetBuffer, packet.GetLength());

    char buff[1024];
    client.GetData(buff);
    PktDef responsePacket(buff);

    if (responsePacket.cmdPacket.header.Ack == 1) {
        cout << "Good Reponse" << endl;
        if (responsePacket.cmdPacket.header.Drive == 1) {
            cout << "Driving " << printDriveDirection((int)packet.cmdPacket.Data[0]) << " for "
                 << (int)packet.cmdPacket.Data[1] << " seconds at " << (int)packet.cmdPacket.Data[2] <<
                 "% speed" << endl;
            responsePacket.DisplayGoodAckNoStatus(std::cout);
        }
        else if (responsePacket.cmdPacket.header.Sleep == 1) {
            responsePacket.DisplayGoodAckNoStatus(std::cout);
        }
        else if (responsePacket.cmdPacket.header.Status == 1) {
            char buffTele[1024];
            client.GetData(buffTele);
            PktDef telemetryPacket(buffTele);
            telemetryPacket.DisplayTelemetry(std::cout);
        }
    }

    return 1;
}

string printDriveDirection(int dir) {
    switch (dir) {
        case 1:
            return "FOWARD";
        case 2:
            return "BACKWARD";
        case 3:
            return "RIGHT";
        case 4:
            return "LEFT";
        default:
            return "UNKNOWN";
    }
}