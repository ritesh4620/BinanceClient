cmake_minimum_required(VERSION 3.16)
project(BinanceClient)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    main.cpp
    OrderBook/OrderBook.cpp
    CSVWriter/CSVWriter.cpp
)

# Add executable
add_executable(BinanceClient ${SOURCES})

# Include directories (so main.cpp can see OrderBook.hpp)
target_include_directories(BinanceClient PRIVATE
    OrderBook
    CSVWriter
)

# Find required packages
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Link libraries
target_link_libraries(BinanceClient PRIVATE
    Boost::system
    Boost::thread
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
)
