#include "Packet.h"

using namespace std;

PktDef::PktDef() : RawBuffer(nullptr), FORWARD(1), BACKWARD(2), LEFT(3), RIGHT(4) {
    // TODO A default constructor that places the PktDef object in a safe state, defined as follows:

    // - All Header information set to zero
    memset(&cmdPacket.header, 0, sizeof(cmdPacket.header));

    // - Data pointer set to nullptr
    cmdPacket.Data = nullptr;

    // - CRC set to zero
    memset(&cmdPacket.CRC, 0, sizeof(cmdPacket.CRC));

    // default setting of the ack flag
    cmdPacket.header.Ack = 1;
}

// TODO An overloaded constructor that takes a RAW data buffer, parses the
// TODO data and populates the Header, Body, and CRC contents of the PktDef object
PktDef::PktDef(char* buff) : FORWARD(1), BACKWARD(2), LEFT(3), RIGHT(4) {
    int position = 0;

    // Deserialize the header.
    // (Assuming your on-wire header layout is identical to your internal representation.)
    memcpy(&cmdPacket.header, buff, sizeof(cmdPacket.header));
    position += sizeof(cmdPacket.header);

    // Calculate payload size: header.Length is the total packet length (header + payload + CRC)
    int payload = cmdPacket.header.Length - (sizeof(cmdPacket.header) + sizeof(cmdPacket.CRC));
    
    // Based on the payload size determine which packet type it is.
    if (payload <= 0) {
        // Case 1: No payload data.
        cmdPacket.Data = nullptr;
        
    }
    else if (payload == DRIVER_BODYSIZE) { // believe this is never used 
        
        cmdPacket.Data = new char[payload];
        memcpy(cmdPacket.Data, &buff[position], payload);

        memcpy(&driverBody, cmdPacket.Data, payload);
        position += payload;
    }
    else if (payload == TELEMETRY_BODYSIZE) {
        cmdPacket.Data = new char[payload];
        memcpy(cmdPacket.Data, &buff[position], payload);
        memcpy(&telemetryBody, cmdPacket.Data, payload);
        position += payload;
    }
    else {
        // Unknown payload size: you can handle this as an error, set Data to nullptr, etc.
        cmdPacket.Data = nullptr;
    }

    // Finally, read (or recalc) the CRC.
    CalcCRC();
}

string PktDef::CreateResponseMessage(int direction, int duration, int speed, string command) {

    if (!(cmdPacket.header.Ack == 1)) 
        return "NACK Packet";

    std::ostringstream os;

    if (command == "drive") {
        os << "Driving " << printDriveDirection(direction) << 
        " for " << duration << "s at " << speed << "\% speed.";
        return os.str();
    } else if (command == "sleep") {
        os << "Sleeping Robot.";
        return os.str();
    } else {
        os << "Bad Command";
        return os.str();
    }
}

string PktDef::printDriveDirection(int dir) {
    switch (dir) {
        case 1:
            return "forward";
        case 2:
            return "backwards";
        case 3:
            return "right";
        case 4:
            return "left";
        default:
            return "UNKNOWN";
    }
}

void PktDef::Display() {
    int payload = cmdPacket.header.Length - (sizeof(cmdPacket.header) + sizeof(cmdPacket.CRC));
    cout << "Payload size: " << payload << endl;
    switch(payload) {
        case 0:
            DisplayGoodAckNoStatus(std::cout);
            break;
        case 9:
            DisplayTelemetry(std::cout);
            break;
        default:
            cout << "Invalid payload length" << endl;
            break;
    }
}

void PktDef::DisplayTelemetry(std::ostream& os) {
    // Check that the telemetry payload exists.
    if (cmdPacket.Data == nullptr) {
        os << "No telemetry data available.\n";
        return;
    }

    os << "Telemetry Body:\n";
    os << "  LastPktCounter: " << telemetryBody.LastPktCounter << "\n";
    os << "  CurrentGrade:   " << telemetryBody.CurrentGrade << "\n";
    os << "  HitCount:       " << telemetryBody.HitCount << "\n";
    os << "  LastCmd:        " << static_cast<int>(telemetryBody.LastCmd) << "\n";
    os << "  LastCmdValue:   " << static_cast<int>(telemetryBody.LastCmdValue) << "\n";
    os << "  LastCmdSpeed:   " << static_cast<int>(telemetryBody.LastCmdSpeed) << "\n";
}

string PktDef::GetTelemetryData() {
    std::ostringstream os;

    if (cmdPacket.Data == nullptr) {
        os << "No telemetry data available.\n";
        return os.str();
    }

    os << "Telemetry Body:\n";
    os << "  LastPktCounter: " << telemetryBody.LastPktCounter << "\n";
    os << "  CurrentGrade:   " << telemetryBody.CurrentGrade << "\n";
    os << "  HitCount:       " << telemetryBody.HitCount << "\n";
    os << "  LastCmd:        " << static_cast<int>(telemetryBody.LastCmd) << "\n";
    os << "  LastCmdValue:   " << static_cast<int>(telemetryBody.LastCmdValue) << "\n";
    os << "  LastCmdSpeed:   " << static_cast<int>(telemetryBody.LastCmdSpeed) << "\n";

    return os.str();
}

void PktDef::DisplayGoodAckNoStatus(std::ostream& os) {
    os << std::dec;
    os << "Packet Count:  " << (int)cmdPacket.header.PktCount << std::endl;
    os << "Drive Flag:    " << (int)cmdPacket.header.Drive << std::endl;
    os << "Stats Flag:    " << (int)cmdPacket.header.Status << std::endl;
    os << "Sleep Flag:    " << (int)cmdPacket.header.Sleep << std::endl;
    os << "Ack Flag:      " << (int)cmdPacket.header.Ack << std::endl;
    os << "Length:        " << (int)cmdPacket.header.Length << std::endl;

    os << endl;
    os << "CRC:           " << (int)cmdPacket.CRC << std::endl;
}

// TODO *************** GETTERS **************************

PktDef::CmdType PktDef::GetCmd() {
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
    return RESPONSE;
}

// TODO A query function that returns true/false based on the Ack flag in the header
bool PktDef::GetAck() { return cmdPacket.header.Ack; }

// TODO a query function that returns the packet Length in bytes
unsigned int PktDef::GetLength() { return cmdPacket.header.Length; }

// TODO a query function that returns a pointer to the objects Body field
char* PktDef::GetBodyData() { return cmdPacket.Data; }

// TODO a query function that returns the PktCount value
unsigned int PktDef::GetPktCount() { return cmdPacket.header.PktCount; }

// TODO ******************* SETTERS *****************************

void PktDef::SetCmd(CmdType commandType) {
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
void PktDef::SetBodyData(char* data, int size) {
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
    cmdPacket.header.Length = sizeof(cmdPacket.header) + size + sizeof(cmdPacket.CRC);
}

// TODO setter that sets the PktCount variable
void PktDef::SetPktCount(int pktCount) { cmdPacket.header.PktCount = pktCount; }

// TODO ********************* OTHERS ***************************

// TODO a function that takes a pointer to a RAW data buffer, the
// TODO size of the buffer in bytes, and calculates the CRC. If the calculated CRC matches the
// TODO CRC of the packet in the buffer the function returns TRUE, otherwise FALSE.

bool PktDef::CheckCRC(char* buff, int size) {
    int packetSize = size - sizeof(cmdPacket.CRC);

    unsigned char packetCRC = static_cast<unsigned char>(buff[size - sizeof(cmdPacket.CRC)]);

    int calculatedCRC = 0;
    for (int i = 0; i < packetSize; i++) {
        unsigned char byte = buff[i];
        while (byte) {
            calculatedCRC += byte & 1;
            byte >>= 1;
        }
    }

    // returns true if they are equal
    return (calculatedCRC == packetCRC);
}

void PktDef::CalcCRC() {
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
    // if there is data we are going to check it if not it will have no effect on CRC number
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

char* PktDef::GenPacket() {
    // TODO a function that allocates the private RawBuffer and transfers the
    // contents from the objects member variables into a RAW data packet (RawBuffer) for
    // transmission. The address of the allocated RawBuffer is returned.
    int headerSize = sizeof(cmdPacket.header);
    int crcSize = sizeof(cmdPacket.CRC);
    int dataSize = cmdPacket.header.Length - headerSize - crcSize;

    int totalSize = headerSize + dataSize + crcSize;

    if (RawBuffer) {
        delete[] RawBuffer;
        RawBuffer = nullptr;
    }

    RawBuffer = new char[totalSize];
    int position = 0;

    memcpy(&RawBuffer[position], &cmdPacket.header, headerSize);
    position += headerSize;

    //memcpy(&RawBuffer[position], &cmdPacket.header, headerSize - 1);
    //position += headerSize - 1;

    //RawBuffer[position++] = static_cast<char>(cmdPacket.header.Length & 0xFF);

    if (cmdPacket.Data != nullptr) {
        memcpy(&RawBuffer[position], cmdPacket.Data, dataSize);
        position += dataSize;
    }

    CalcCRC();
    memcpy(&RawBuffer[position], &cmdPacket.CRC, crcSize);

    return RawBuffer;
}
