#ifndef ROBOTTEST_PACKET_H
#define ROBOTTEST_PACKET_H

#include <iostream>
#include <stdio.h>
#include <string.h>

class PktDef {
public:

    enum CmdType {
        DRIVE = 1,
        RESPONSE = 2,
        SLEEP = 3
    };

    struct Header {
        unsigned short int PktCount;     // 2 bytes
        unsigned char Drive : 1;   // 1 bit
        unsigned char Status : 1;  // 1 bit
        unsigned char Sleep : 1;   // 1 bit   --> 1 byte
        unsigned char Ack : 1;     // 1 bit
        unsigned char Padding : 4; // 4 bits
        //unsigned short int Length;   // 8 bits
        unsigned char Length;
    };

    struct DriverBody {
        unsigned char Direction; // 1 byte
        unsigned char Duration;  // 1 byte
        unsigned char Speed;     // 1 byte
    };

    // TelemetryBody: defined according to protocol (9 bytes total)
    struct TelemetryBody {
        unsigned short int LastPktCounter; // 2 bytes
        unsigned short int CurrentGrade;   // 2 bytes
        unsigned short int HitCount;       // 2 bytes
        unsigned char LastCmd;             // 1 byte
        unsigned char LastCmdValue;        // 1 byte
        unsigned char LastCmdSpeed;        // 1 byte
    };

private:

    struct CmdPacket {
        Header header; // 9 bytes - 12 with padding?
        char* Data;    //
        unsigned char CRC;      // 1 byte
    };

public:

    CmdPacket cmdPacket;
    char* RawBuffer;

    const int FORWARD = 1;
    const int BACKWARD = 2;
    const int RIGHT = 3;
    const int LEFT = 4;

    static const int HEADERSIZE = 4;
    static const int CRCSIZE = 1;
    static const int DRIVER_BODYSIZE = 3;
    static const int TELEMETRY_BODYSIZE = 9;

    DriverBody driverBody;

    TelemetryBody telemetryBody;

    PktDef();
    PktDef(char* buff);

    void DisplayTelemetry(std::ostream& os);

    void DisplayGoodAckNoStatus(std::ostream& os);

    void Display(std::ostream& os);

    // TODO *************** GETTERS **************************

    CmdType GetCmd();

    // TODO A query function that returns true/false based on the Ack flag in the header
    bool GetAck();

    // TODO a query function that returns the packet Length in bytes
    unsigned int GetLength();

    // TODO a query function that returns a pointer to the objects Body field
    char* GetBodyData();

    // TODO a query function that returns the PktCount value
    unsigned int GetPktCount();

    // TODO ******************* SETTERS *****************************

    void SetCmd(CmdType commandType);

    // TODO a set function that takes a pointer to a RAW data buffer
    // TODO and the size of the buffer in bytes. This function will allocate the packets Body field and
    // TODO copies the provided data into the objects buffer
    void SetBodyData(char* data, int size);

    // TODO setter that sets the PktCount variable
    void SetPktCount(int pktCount);

    // TODO ********************* OTHERS ***************************

    // TODO a function that takes a pointer to a RAW data buffer, the
    // TODO size of the buffer in bytes, and calculates the CRC. If the calculated CRC matches the
    // TODO CRC of the packet in the buffer the function returns TRUE, otherwise FALSE.
    bool CheckCRC(char* buff, int size);

    void CalcCRC();

    char* GenPacket();

};


#endif