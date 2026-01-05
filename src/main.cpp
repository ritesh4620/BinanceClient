#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <boost/asio/ssl/context.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include "OrderBook.hpp"
#include "CSVWriter.hpp"


CSVWriter writer("/mnt/d/Source_Code/Binance_Project/src/Files");
struct Trade {
    double price = 0;
    double quantity = 0;
};

struct SymbolData {
    OrderBook orderBook;
    Trade lastTrade;
    double lastTickerPrice = 0;
    std::mutex mtx;  // For trades and ticker updates
};

std::map<std::string, SymbolData> symbols;
std::mutex symbols_mutex;



typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;

context_ptr on_tls_init(websocketpp::connection_hdl) {
    auto ctx = std::make_shared<boost::asio::ssl::context>(
        boost::asio::ssl::context::tlsv12_client
    );
    ctx->set_verify_mode(boost::asio::ssl::verify_none);
    return ctx;
}

void printSummary() 
{
    double bidPrice,bidQty,askPrice,askQty;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));

        std::lock_guard<std::mutex> mapLock(symbols_mutex);
        std::cout << "Symbols size: " << symbols.size() << std::endl;

        for (auto &[symbol, data] : symbols) {
            std::lock_guard<std::mutex> lock(data.mtx);

            if (!data.orderBook.isInitialized())
                continue;
            data.orderBook.topBid(bidPrice,bidQty);
            data.orderBook.topAsk(askPrice,askQty);
            std::cout << symbol
                      << " | LastTrade=" << data.lastTrade.price
                      << " | LastTicker=" << data.lastTickerPrice
                      << " | TopBidPrice=" <<bidPrice<<" TopBidQty="<<bidQty 
                      << " | TopAsk=" <<askPrice<<" TopAskQty="<<askQty
                      << std::endl;
        }
    }
}


void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
    auto j = nlohmann::json::parse(msg->get_payload());
    //std::cout<<"Raw packet: "<<j<<std::endl;
    const std::string& stream = j["stream"];
    const auto& data = j["data"];
    
    std::string symbol = data["s"]; // symbol name, e.g., "BNBUSDT"

    // Lock if using multi-threaded context
    std::lock_guard<std::mutex> lock(symbols_mutex);

    auto& symData = symbols[symbol]; // This creates a new OrderBook if it doesn't exist
    uint64_t ts = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch()
              ).count();
    if (stream.find("@trade") != std::string::npos) {
       // std::cout << "[TRADE] "<< data["s"] << " p=" << data["p"]<< " q=" << data["q"] << std::endl;
         symData.lastTrade.price =
        std::stod(data["p"].get<std::string>());
        symData.lastTrade.quantity =
        std::stod(data["q"].get<std::string>());
        writer.writeTrade(symbol,
                      symData.lastTrade.price,
                      symData.lastTrade.quantity,
                      ts);
    }
    else if (stream.find("@ticker") != std::string::npos) {
       // std::cout << "[TICKER] "<< data["s"] << " last=" << data["c"] << std::endl;
          symData.lastTickerPrice =
        std::stod(data["c"].get<std::string>());
         writer.writeTicker(symbol,
                       symData.lastTickerPrice,
                       ts);
    }
    else if (stream.find("@depth") != std::string::npos) {
        //std::cout << "[DEPTH] "<< data["s"]<< " bids=" << data["b"].size()<< " asks=" << data["a"].size() << std::endl;
        symData.orderBook.updateFromDepth(data);
        writer.writeOrderBook(symbol,
                          symData.orderBook,
                          ts);
    }
   /* if (!symData.orderBook.bids.empty()) 
    { 
        auto it = symData.orderBook.bids.begin();  
    }
    else if (stream.find("@depth") != std::string::npos) {
    

    std::cout << "[DEPTH] "
              << symbol
              << " bids=" << data["b"].size()
              << " asks=" << data["a"].size()
              << " | OB bids=" << symData.orderBook.bids.size()
              << " asks=" << symData.orderBook.asks.size()
              << std::endl;
}*/
   
  // printSummary();
}



int main() 
{
    client c;

    c.init_asio();
    c.set_tls_init_handler(&on_tls_init);
    c.set_message_handler(&on_message);

    std::thread(printSummary).detach();
    std::string uri =
       "wss://stream.binance.com:9443/stream?streams="
        "bnbusdt@trade/bnbusdt@ticker/bnbusdt@depth@100ms/"
        "btcusdt@trade/btcusdt@ticker/btcusdt@depth@100ms/"
        "ethusdt@trade/ethusdt@ticker/ethusdt@depth@100ms/"
        "adausdt@trade/adausdt@ticker/adausdt@depth@100ms/"
        "xrpusdt@trade/xrpusdt@ticker/xrpusdt@depth@100ms/"
        "solusdt@trade/solusdt@ticker/solusdt@depth@100ms/"
        "dotusdt@trade/dotusdt@ticker/dotusdt@depth@100ms/"
        "maticusdt@trade/maticusdt@ticker/maticusdt@depth@100ms/"
        "dogeusdt@trade/dogeusdt@ticker/dogeusdt@depth@100ms/"
        "ltcusdt@trade/ltcusdt@ticker/ltcusdt@depth@100ms";

    websocketpp::lib::error_code ec;
    auto con = c.get_connection(uri, ec);
    if (ec) {
        std::cerr << "Connection error: " << ec.message() << std::endl;
        return 1;
    }

    c.connect(con);
    c.run();
}
