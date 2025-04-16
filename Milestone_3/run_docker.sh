#!/bin/bash
# script to build and run the Docker container for Milestone 3

# Stop any containers using port 23500
echo "Stopping any containers using port 23500..."
docker stop $(docker ps -q --filter publish=23500) 2>/dev/null || true

# Build the Docker image
echo "Building Docker image..."
docker build -t milestone3-robot-control .

# Run the Docker container
echo "Running Docker container..."
docker run -p 23500:23500 --rm milestone3-robot-control