cmake_minimum_required(VERSION 3.30)
project(RobotTest)

set(CMAKE_CXX_STANDARD 20)

add_executable(RobotTest main.cpp
                            MySocket/MySocket.cpp
                            Packet/Packet.cpp)
