cmake_minimum_required(VERSION 3.30)
project(Milestone2)

set(CMAKE_CXX_STANDARD 20)

add_executable(TCPServer TCPServer.cpp MySocket.cpp)
add_executable(TCPClient TCPClient.cpp MySocket.cpp)

add_executable(UDPServer UDPServer.cpp MySocket.cpp)
add_executable(UDPClient UDPClient.cpp MySocket.cpp)