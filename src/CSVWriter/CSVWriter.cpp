#include "CSVWriter.hpp"
#include <iomanip>
CSVWriter::CSVWriter(const std::string& filePath)
{
    tradeFile.open(filePath + "/trades.csv", std::ios::out | std::ios::app);
    tickerFile.open(filePath + "/ticker.csv", std::ios::out | std::ios::app);
    depthFile.open(filePath + "/orderbook_l2.csv", std::ios::out | std::ios::app);

    tradeFile  << "ts,symbol,price,qty\n";
    tickerFile << "ts,symbol,last_price\n";
    depthFile  << "ts,symbol,side,price,qty\n";
}
CSVWriter::~CSVWriter() {
    tradeFile.close();
    tickerFile.close();
    depthFile.close();
}

void CSVWriter::writeTrade(const std::string& symbol,double price,double qty,uint64_t ts)
{
    std::lock_guard<std::mutex> lock(mtx);
    tradeFile << ts << "," << symbol << ","
              << price << "," << qty << "\n";
}

void CSVWriter::writeTicker(const std::string& symbol,double last,uint64_t ts)
{
    std::lock_guard<std::mutex> lock(mtx);
    tickerFile << ts << "," << symbol << ","
               << last << "\n";
}

void CSVWriter::writeOrderBook(const std::string& symbol,const OrderBook& ob,uint64_t ts)
{
    std::lock_guard<std::mutex> lock(mtx);

    {
        std::lock_guard<std::mutex> obLock(ob.mtx);

        for (const auto& [price, qty] : ob.bids) {
            depthFile << ts << "," << symbol
                     << ",B," << price << "," << qty << "\n";
        }

        for (const auto& [price, qty] : ob.asks) {
            depthFile << ts << "," << symbol
                     << ",A," << price << "," << qty << "\n";
        }
    }
}
