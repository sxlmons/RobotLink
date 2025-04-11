#include "MySocket.h"

int main() {

    MySocket server(SERVER, "127.0.0.1", 27000, TCP, 1024);

    server.ConnectTCP();

    char buffer[1024] = {};
    int bytesReceived = server.GetData(buffer);
    std::cout << "Received from client: " << std::string(buffer, bytesReceived) << std::endl;

    const char* reply = "Hello from server!";
    server.SendData(reply, strlen(reply));

    server.DisconnectTCP();

    return 1;
}