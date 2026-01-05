#pragma once 
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include "OrderBook.hpp"
class CSVWriter
{
public:
    explicit CSVWriter(const std::string& filePath);
    ~CSVWriter();
    void writeTrade(const std::string& symbol,
                    double price,
                    double qty,
                    uint64_t ts);

    void writeTicker(const std::string& symbol,
                     double last,
                     uint64_t ts);

    void writeOrderBook(const std::string& symbol,
                        const OrderBook& ob,
                        uint64_t ts);
private:
    std::ofstream tradeFile;
    std::ofstream tickerFile;
    std::ofstream depthFile;
    std::mutex mtx;
};