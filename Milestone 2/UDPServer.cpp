#include "MySocket.h"

int main() {

    MySocket server(SERVER, "127.0.0.1", 27001, UDP, 1024);

    char buffer[1024];
    int bytesReceived = server.GetData(buffer);
    std::cout << "Received from client: " << std::string(buffer, bytesReceived) << std::endl;

    const char* response = "Hello UDP Client!";
    server.SendData(response, strlen(response));

    return 1;
}