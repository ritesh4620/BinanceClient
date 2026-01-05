1. Overview
.This project is a C++ Binance WebSocket client that subscribes to real-time market data streams (Trades, Ticker, Depth) and builds an L2 order book while persisting data to CSV files.
.It is designed with:
    .Low-latency market data handling
    .Thread-safe order book updates
    .Modular architecture (OrderBook, CSVWriter, Client)

2. System Requirements
.OS: Ubuntu 22.04 LTS
. Hypervisor vendor: Windows Subsystem for Linux
.CPU: Intel(R) Core(TM) i5-5300U CPU @ 2.30GHz
.Byte Order: Little Endian
.RAM: Minimum 4 GB (6 GB recommended)
.Compiler: g++ (C++17 or later)

3. Dependencies
.Ensure the following libraries are installed before building:
.Boost
.WebSocket++
.OpenSSL
.nlohmann/json
.CMake (>= 3.16)

4. Install Dependencies
sudo apt update
sudo apt install -y 
    build-essential \
    cmake \
    libboost-all-dev \
    libssl-dev \
    nlohmann-json3-dev

5.Build Instructions
.Clone the repository:
    git clone https://github.com/ritesh4620/BinanceClient.git
    cd BinanceClient
.Create a build directory:
    mkdir build
.Run CMake and compile:
    cmake ..
    make clean
    make
.Running the Application
    After successful compilation, a binary named BinanceClient will be generated.
    ./BinanceClient