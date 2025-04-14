#include "MySocket/MySocket.h"
#include "Packet/Packet.h"

#include "stdbool.h"

using namespace std;

string printDriveDirection(int dir);

int main() {

    int packetCount = 1;
    int command = 0;

    while(1) {
        cout << "Command (1 Drive, 2 Sleep, 3 Response" << endl;
        cin >> command;

        MySocket client(CLIENT, "127.0.0.1", 27000, UDP, 1024);

        PktDef packet;
        packet.SetPktCount(packetCount);

        bool breakLoop = false;

        switch (command) {
            case 1:
                packet.SetCmd(PktDef::DRIVE);
                break;
            case 2:
                packet.SetCmd(PktDef::SLEEP);
                break;
            case 3:
                packet.SetCmd(PktDef::RESPONSE);
                break;
            default:
                breakLoop = true;
        }

        if(breakLoop)
            break;

        packet.driverBody.Direction = 1;
        packet.driverBody.Duration = 10;
        packet.driverBody.Speed = 80;
        packet.SetBodyData(reinterpret_cast<char *>(&packet.driverBody), sizeof(packet.driverBody));

        char *packetBuffer = packet.GenPacket();

        client.SendData(packetBuffer, packet.GetLength());

        char buff[1024];
        client.GetData(buff);
        PktDef responsePacket(buff);

        if (responsePacket.cmdPacket.header.Ack == 1) {
            packetCount++;
            cout << "Good Reponse" << endl;
            if (responsePacket.cmdPacket.header.Drive == 1) {
                cout << "Driving " << printDriveDirection((int) packet.cmdPacket.Data[0]) << " for "
                     << (int) packet.cmdPacket.Data[1] << " seconds at " << (int) packet.cmdPacket.Data[2] <<
                     "% speed" << endl;
                responsePacket.DisplayGoodAckNoStatus(std::cout);
            } else if (responsePacket.cmdPacket.header.Sleep == 1) {
                responsePacket.DisplayGoodAckNoStatus(std::cout);
                return 0;
            } else if (responsePacket.cmdPacket.header.Status == 1) {
                char buffTele[1024];
                client.GetData(buffTele);
                PktDef telemetryPacket(buffTele);
                telemetryPacket.DisplayTelemetry(std::cout);
                packetCount++;
            }
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