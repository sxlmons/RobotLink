#include "MySocket.h"

int main() {

    MySocket client(CLIENT, "127.0.0.1", 27000, TCP, 1024);

    client.ConnectTCP();

    const char* message = "Hello from client!";
    client.SendData(message, strlen(message));

    char buffer[1024];
    int bytes = client.GetData(buffer);
    std::cout << "Server said: " << std::string(buffer, bytes) << std::endl;

    client.DisconnectTCP();

    return 1;
}