# CSCN72050 Term Project

This repository contains the source code and documentation for my **CSCN72050 Term Project**, which involves creating a communication protocol and a system for controlling a mobile robot. The project is divided into multiple milestones, each progressively adding more functionality to the system, including packet handling, socket communication, and a RESTful web interface for robot control.

---

## Milestone #1: PktDef Class - Protocol Definition

### Objective
In this milestone, the **PktDef** class defines the structure and functionality for handling robot communication. It generates and parses packets that include headers, commands, and CRC for data integrity.

### PktDef Class Features
- **Header**: Contains packet count, flags (Drive, Status, Sleep, Ack), padding, and length.
- **DriveBody**: Contains parameters for the Drive command (Direction, Duration, Speed).
- **CRC Calculation**: A simple checksum for packet validation.
- **Commands**: Includes Drive, Sleep, and Response commands.
- **Data Buffer**: Raw data is serialized into a buffer for transmission.

### Unit Testing
MSTest unit tests are provided to verify the functionality of the **PktDef** class, covering all necessary methods and edge cases.

## Milestone #2: MySocket Class - TCP/UDP Socket Communication

### Objective
This milestone introduces the **MySocket** class, which provides functionality for creating and managing socket connections over TCP and UDP protocols. It can operate as either a Client or Server, handling both sending and receiving of data.

### MySocket Class Features
- **SocketType**: Defines whether the socket is a CLIENT or SERVER.
- **ConnectionType**: Specifies whether the socket is using TCP or UDP.
- **Buffer**: Dynamically allocated buffer for data communication.
- **TCP Communication**: Functions to establish, send data, and disconnect a TCP/IP connection.
- **UDP Communication**: Functions to send and receive data without establishing a connection.
- **Port & IP Address Management**: Methods to configure and query the IP address and port number.

### Unit Testing
MSTest unit tests are provided to validate the functionality of the **MySocket** class, ensuring correct handling of different connection types and buffer management.

## Milestone #3: RESTful Web Server for Command and Control GUI

### Objective
In this milestone, we integrate the **PktDef** and **MySocket** classes into a RESTful web server application that allows a web browser to control the robot using HTTP requests. The web server runs in a Docker container and communicates with the robot using UDP/IP packets.

### Command and Control GUI Features
- **Docker Container**: The web server runs inside a Docker container for ease of deployment.
- **Routes**:
  - `/`: Returns the main GUI interface for robot control.
  - `/connect`: Takes IP address and port number to configure communication with the robot.
  - `/telecommand/`: Sends Drive or Sleep commands to the robot.
  - `/telementry_request/`: Sends a request for housekeeping telemetry.

### Communication
- The web browser sends HTTP requests to the server.
- The server sends UDP/IP packets to the robot for command execution.

### Running the Docker Container

To access the Docker image and run the web server for the Command and Control GUI, use the following terminal command:

```bash
docker run -ti --rm -v '/path/to/your/Milestone3':/shared -p 23500:23500 'your_image_name' bash
```

### Simulation Software
The **Robot_Simulator.exe** simulates the robot's behavior. It listens for commands and validates the received data.

#### Instructions to run the simulator:
1. From the Windows command prompt, run **Robot_Simulator.exe** with a port number argument.
2. Start the Docker container with the Command and Control GUI software.
3. Open the web browser and interact with the GUI.

## Getting Started

### Prerequisites
- **C++20**: The project uses C++20 features, so ensure your environment is configured accordingly.
- **Docker**: Docker must be installed for running the web server in a container.
- **Robot Simulator**: Make sure the **Robot_Simulator.exe** is running on the PC for testing.
  - **Note**: The **Robot Simulator** is a Windows application only.

### Cloning the Repository
To clone this repository, use the following command:
```bash
git clone https://github.com/sxlmons/RobotLink.git
```

