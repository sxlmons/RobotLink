#include "MySocket.h"

int main() {

    MySocket client(CLIENT, "127.0.0.1", 27001, UDP, 1024);

    const char* msg = "Hello UDP Server!";
    client.SendData(msg, strlen(msg));

    char buffer[1024];
    int bytes = client.GetData(buffer);
    std::cout << "Server said: " << std::string(buffer, bytes) << std::endl;

    return 1;
}