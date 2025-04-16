FROM ubuntu:22.04

# Avoid prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install minimal dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libboost-system-dev \
    libboost-filesystem-dev \
    libasio-dev \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy only required files
COPY CMakeLists.txt /app/
COPY main.cpp /app/
COPY Packet /app/Packet/
COPY MySocket /app/MySocket/
COPY public /app/public/
COPY crow_all.h /app/

# Fix paths in main.cpp for Docker environment
RUN sed -i 's|"../public/Pages/index.html"|"/app/public/Pages/Index.html"|g' main.cpp && \
    sed -i 's|"../public/Pages/"|"/app/public/Pages/"|g' main.cpp && \
    sed -i 's|"../public/Images/"|"/app/public/Images/"|g' main.cpp && \
    sed -i 's|"../public/Scripts/"|"/app/public/Scripts/"|g' main.cpp

# Create build directory and build the project
RUN mkdir -p build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Create symbolic link for public directory
RUN cd build && ln -sf /app/public ./public

# Expose port
EXPOSE 23500

# Set the working directory to the build directory
WORKDIR /app/build

# Run the server
CMD ["./MS3"]