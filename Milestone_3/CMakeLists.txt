cmake_minimum_required(VERSION 3.7)
project(Milestone3)

# Use C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(Threads REQUIRED)

# Include directories
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Define source files
set(SOURCE_FILES
        main.cpp
        Packet/Packet.cpp
        MySocket/MySocket.cpp
)

# Define executable
add_executable(MS3 ${SOURCE_FILES})

# Link libraries
target_link_libraries(MS3 PRIVATE
        ${Boost_LIBRARIES}
        Threads::Threads
)

# Print minimal build information
message(STATUS "Configured ${PROJECT_NAME} with C++${CMAKE_CXX_STANDARD}")