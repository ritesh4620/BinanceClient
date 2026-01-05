#pragma once
#include <map>
#include <mutex>
#include <vector>
#include <string>
#include <cstdint>
#include <functional>
#include <nlohmann/json.hpp>

class OrderBook {
public:
    OrderBook() : last_update_id(0), initialized(false) {}

    // Apply a depth update
    void update(const std::vector<std::pair<double,double>> &bids,
                const std::vector<std::pair<double,double>> &asks,
                uint64_t updateId);

    void topBid(double &bidPrice, double& bidQty) const;
    void topAsk(double &askPrice, double& askQty) const;

    uint64_t getLastUpdateId() const { return last_update_id; }
    bool isInitialized() const { return initialized; }
    void updateFromDepth(const nlohmann::json& data);
    std::map<double, double, std::greater<>> bids; // highest first
    std::map<double, double> asks;                 // lowest first
    uint64_t last_update_id;
    bool initialized;
    mutable std::mutex mtx;

     


};
