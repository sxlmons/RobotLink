#!/bin/bash
# Script to clean build artifacts from repository

echo "Cleaning repository of build artifacts..."

# Remove CMake build directories
find . -name "CMakeFiles" -type d -exec rm -rf {} +
find . -name "cmake-build-debug" -type d -exec rm -rf {} +
find . -name "cmake-build-release" -type d -exec rm -rf {} +

# Remove binary files
find . -name "*.o" -type f -delete
find . -name "*.obj" -type f -delete
find . -name "*.exe" -type f -delete
find . -name "*.out" -type f -delete
find . -name "*.a" -type f -delete
find . -name "*.so" -type f -delete
find . -name "*.dll" -type f -delete
find . -name "*.dylib" -type f -delete

# Remove debug symbols
find . -name "*.dSYM" -type d -exec rm -rf {} +
find . -name "*.pdb" -type f -delete
find . -name "*.idb" -type f -delete

# Remove CMake generated files
find . -name "CMakeCache.txt" -type f -delete
find . -name "cmake_install.cmake" -type f -delete
find . -name "Makefile" -type f -delete

# Remove manually built executables from milestone directories
rm -f Milestone1/Client Milestone1/Server
rm -f Milestone2/TCPClient Milestone2/TCPServer Milestone2/UDPClient Milestone2/UDPServer
rm -f Milestone3/MS3
rm -f RobotTest/RobotTest

echo "Repository cleaned!"