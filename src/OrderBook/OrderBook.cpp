#include "OrderBook.hpp"

void OrderBook::update(const std::vector<std::pair<double,double>> &newBids,
                       const std::vector<std::pair<double,double>> &newAsks,
                       uint64_t updateId)
{
    std::lock_guard<std::mutex> lock(mtx);

    // ignore old updates
    if(initialized && updateId <= last_update_id) return;

    for(auto &[price, qty] : newBids) {
        if(qty == 0) bids.erase(price);
        else bids[price] = qty;
    }

    for(auto &[price, qty] : newAsks) {
        if(qty == 0) asks.erase(price);
        else asks[price] = qty;
    }

    last_update_id = updateId;
    initialized = true;
}

void OrderBook::topBid(double &bidPrice, double& bidQty) const 
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        bidPrice = bids.begin()->first;
        bidQty = bids.begin()->second;
//    return bids.empty() ? 0.0 : bids.begin()->first;
    }
}

void  OrderBook::topAsk(double& askPrice, double& askQty ) const 
{
    {
    std::lock_guard<std::mutex> lock(mtx);
    //return asks.empty() ? 0.0 : asks.begin()->first;
    askPrice = asks.begin()->first;
    askQty = asks.begin()->second;
    }
}

void OrderBook::updateFromDepth(const nlohmann::json& data) {
        const auto& bidArr = data["b"];
        const auto& askArr = data["a"];

        bids.clear();
        asks.clear();

        for (const auto& b : bidArr) {
            double price = std::stod(b[0].get<std::string>());
            double qty   = std::stod(b[1].get<std::string>());
            bids[price] = qty;
        }

        for (const auto& a : askArr) {
            double price = std::stod(a[0].get<std::string>());
            double qty   = std::stod(a[1].get<std::string>());
            asks[price] = qty;
        }

        initialized = true;
    }