#ifndef MILESTONE_1_PACKET_H
#define MILESTONE_1_PACKET_H

#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

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
        unsigned short int Length;   // 8 bits ??
    };

    struct DriverBody {
        unsigned char Direction; // 1 byte
        unsigned char Duration;  // 1 byte
        unsigned char Speed;     // 1 byte
    };

private:

    struct CmdPacket {
        Header header; // 9 bytes - 12 with padding?
        char* Data;    //
        char CRC;      // 1 byte
    };

public:

    CmdPacket cmdPacket;
    char* RawBuffer;
    //const int EmptyPktSize = 10; // 10 bytes

    const int FORWARD;
    const int BACKWARD;
    const int LEFT;
    const int RIGHT;
    //const unsigned int HEADERSIZE = 0; // ???

    DriverBody driverBody; // ???

    PktDef() : RawBuffer(nullptr), FORWARD(1), BACKWARD(2), LEFT(3), RIGHT(4) {
        // TODO A default constructor that places the PktDef object in a safe state, defined as follows:

        // - All Header information set to zero
        memset(&cmdPacket.header, 0, sizeof(cmdPacket.header));

        // - Data pointer set to nullptr
        cmdPacket.Data = nullptr;

        // - CRC set to zero
        memset(&cmdPacket.CRC, 0, sizeof(cmdPacket.CRC));

        // default setting of the ack flag
        // probably better to do a different way
        cmdPacket.header.Ack = 1;
    }

// TODO An overloaded constructor that takes a RAW data buffer, parses the
// TODO data and populates the Header, Body, and CRC contents of the PktDef object
    PktDef(char* buff) : FORWARD(1), BACKWARD(2), LEFT(3), RIGHT(4) {

        // track the position of where the buffer is in memory
        int position = 0;

        // set the cmd packet header to the 0th position onward to the size of the header in the class
        memcpy(&cmdPacket.header, &buff[position], sizeof(cmdPacket.header));
        // move the position down the size of the header
        position += sizeof(cmdPacket.header);

        // calculate the size of the payload. only either 0 or 3 i believe
        unsigned long payload = cmdPacket.header.Length - sizeof(cmdPacket.header) - sizeof(cmdPacket.CRC);
        // allocate memory to that size
        cmdPacket.Data = new char[payload];
        // copy the memory from the buff at the current position onward for the length of the payload
        memcpy(&cmdPacket.Data, &buff[position], payload);

        // calculate this packets CRC
        CalcCRC();
    }

// Function that can be called after a packet is constructed to print out all values in it
    void Display(std::ostream& os) {

        os << std::dec;
        os << "Packet Count:  " << (int)cmdPacket.header.PktCount << std::endl;
        os << "Drive Flag:    " << (int)cmdPacket.header.Drive << std::endl;
        os << "Stats Flag:    " << (int)cmdPacket.header.Status << std::endl;
        os << "Sleep Flag:    " << (int)cmdPacket.header.Sleep << std::endl;
        os << "Ack Flag:      " << (int)cmdPacket.header.Ack << std::endl;
        os << "Length:        " << (int)cmdPacket.header.Length << std::endl;
        os << endl;
        os << "Direction:     " << (int)cmdPacket.Data[0] << std::endl;
        os << "Duration:      " << (int)cmdPacket.Data[1] << std::endl;
        os << "Speed:         " << (int)cmdPacket.Data[2] << std::endl;
        os << endl;
        os << "CRC:           " << (int)cmdPacket.CRC << std::endl;
    }

// TODO *************** GETTERS **************************

    CmdType GetCmd() {
        // TODO a query function that returns the CmdType based on the set command flag bit
        // initialize a variable that stores the command flag bit in context
        // In a list match the command flag bit to the corresponding command
        // return the command

        // checks each flag and sets the command accordingly
        if (cmdPacket.header.Drive == 1)
            return DRIVE;
        else if (cmdPacket.header.Status == 1)
            return RESPONSE;
        else if (cmdPacket.header.Sleep == 1)
            return SLEEP;
        // maybe needs a default case
    }

// TODO A query function that returns true/false based on the Ack flag in the header
    bool GetAck() { return cmdPacket.header.Ack; }

// TODO a query function that returns the packet Length in bytes
    unsigned int GetLength() { return cmdPacket.header.Length; }

// TODO a query function that returns a pointer to the objects Body field
    char* GetBodyData() { return cmdPacket.Data; }

// TODO a query function that returns the PktCount value
    unsigned int GetPktCount() { return cmdPacket.header.PktCount; }

// TODO ******************* SETTERS *****************************

    void SetCmd(CmdType commandType) {
        // TODO A set function that sets the packets command flag based on
        // TODO the CmdType

        // sets the command flag based on the input command
        switch (commandType) {
            case DRIVE:
                cmdPacket.header.Drive = 1;
                break;
            case RESPONSE:
                cmdPacket.header.Status = 1;
                break;
            case SLEEP:
                cmdPacket.header.Sleep = 1;
            default:
                break;
        }
    }

// TODO a set function that takes a pointer to a RAW data buffer
// TODO and the size of the buffer in bytes. This function will allocate the packets Body field and
// TODO copies the provided data into the objects buffer
    void SetBodyData(char* data, int size) {
        // if the drive flag is set to zero there wont be any driving data so we set
        // data to nullptr and the length of packet header + CRC (no data)
        if (cmdPacket.header.Drive == 0) {
            cmdPacket.header.Length = sizeof(cmdPacket.header) + sizeof(cmdPacket.CRC);
            cmdPacket.Data = nullptr;
            return;
        }

        // if there is data in the drive memory wipe it
        if (cmdPacket.Data) {
            delete[] cmdPacket.Data;
            cmdPacket.Data = nullptr;
        }

        // allocate more memory
        cmdPacket.Data = new char[size];

        // copy that memory from the input data to the cmd Packet data the size of the data
        memcpy(cmdPacket.Data, data, size);

        // set the packets length to the int size of header plus the size of the data plus the int size of the CRC
        cmdPacket.header.Length = (int)sizeof(cmdPacket.header) + size + (int)sizeof(cmdPacket.CRC);
    }

// TODO setter that sets the PktCount variable
    void SetPktCount(int pktCount) { cmdPacket.header.PktCount = pktCount; }

// TODO ********************* OTHERS ***************************

// TODO a function that takes a pointer to a RAW data buffer, the
// TODO size of the buffer in bytes, and calculates the CRC. If the calculated CRC matches the
// TODO CRC of the packet in the buffer the function returns TRUE, otherwise FALSE.

// CURRENTLY NOT WORKING
    bool CheckCRC(char* buff, int size) {
        // gets the size of the packet not including the CRC
        int packetSize = size - sizeof(cmdPacket.CRC);

        // gets this input datas CRC for comparison
        char packetCRC = buff[size - sizeof(cmdPacket.CRC)];

        int calculatedCRC = 0;
        for (int i = 0; i < packetSize; i++) {
            // go through each byte of the buff, buff[i] is the char *byte* at that index
            unsigned char byte = buff[i];
            // while that byte still has bits to look at
            while (byte) {
                // at a bit to the CRC if the AND operation is true here
                // if the byte is 1 and we do the AND with the 1 below it will add a 1 to the count
                calculatedCRC += byte & 1;
                // bit shift to the right by 1 to work towards the end of the byte
                byte >>= 1;
            }
        }

        // returns true if they are equal
        return (calculatedCRC == (int)packetCRC);
    }

    void CalcCRC() {
        // TODO a function that calculates the CRC and sets the objects packet CRC parameter.
        int bitCount = 0;

        // casts the header to unsigned char* to go through the bytes
        unsigned char* headerBytes = reinterpret_cast<unsigned char*>(&cmdPacket.header);
        for (size_t i = 0; i < sizeof(cmdPacket.header); i++) {
            // same logic explain above
            unsigned char byte = headerBytes[i];
            while (byte) {
                bitCount += byte & 1;
                byte >>= 1;
            }
        }

        // if there is data we are going to check it if not i will have no effect on CRC number
        if (cmdPacket.Data != nullptr)
            for (int i = 0; i < cmdPacket.header.Length - sizeof(cmdPacket.header) - sizeof(cmdPacket.CRC); i++) {
                unsigned char byte = static_cast<unsigned char>(cmdPacket.Data[i]);
                while (byte) {
                    bitCount += byte & 1;
                    byte >>= 1;
                }
            }

        // CRC is set to the bitCount casted to a char
        cmdPacket.CRC = static_cast<char>(bitCount);
    }

    char* GenPacket() {
        // TODO a function that allocates the private RawBuffer and transfers the
        // contents from the objects member variables into a RAW data packet (RawBuffer) for
        // transmission. The address of the allocated RawBuffer is returned.
        int headerSize = sizeof(cmdPacket.header);
        int dataSize = cmdPacket.header.Length - sizeof(cmdPacket.header) - sizeof(cmdPacket.CRC);
        int crcSize = sizeof(cmdPacket.CRC);

        int totalSize = headerSize + dataSize + crcSize;

        if (RawBuffer) {
            delete[] RawBuffer;
            RawBuffer = nullptr;
        }

        RawBuffer = new char[totalSize];

        int position = 0;

        memcpy(&RawBuffer[position], &cmdPacket.header, headerSize);
        position += headerSize;

        if (cmdPacket.Data != nullptr) {
            memcpy(&RawBuffer[position], &cmdPacket.Data, dataSize);
            position += dataSize;
        }

        CalcCRC();
        memcpy(&RawBuffer[position], &cmdPacket.CRC, crcSize);

        return RawBuffer;
    }

    /*PktDef();
    PktDef(char* buff);

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

    char* GenPacket();*/

};

#endif //MILESTONE_1_PACKET_H
