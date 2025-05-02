#include <iostream>
#include <string>
#include <memory>

struct Trade {
    std::string symbol;
    double price;

    Trade(const std::string& sym, double p)
        : symbol(sym), price(p) {
            std::cout << "Trade created successfully " << symbol << " at " << price << std::endl;
        }

    ~Trade() {
        std::cout << "Trade deleted successfully: " << symbol << std::endl;
    }
};

// PART 3: our own implementation of unique_ptr
class TradeHandle {
    Trade* ptr;

public:
    TradeHandle(Trade* p) :
        ptr(p) {
            std::cout << "TradeHandle took ownership of trade " << ptr << "\n";
        }

    ~TradeHandle() {
        delete ptr;
        std::cout << "TradeHandle deleted trade " << ptr << "\n";
    }

    TradeHandle(const TradeHandle&) = delete;

    TradeHandle& operator=(const TradeHandle&) = delete;

    TradeHandle(TradeHandle&& other) : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    TradeHandle& operator=(TradeHandle&& other) {
        if (this != &other) {
            delete ptr;        // clean up current resource
            ptr = other.ptr;   // take ownership
            other.ptr = nullptr;
        }
        return *this;
    }

    Trade* operator->() {
        return ptr;
    }

    Trade& operator*() {
        return *ptr;
    }
};

int main() {

    // code for PART 1
    Trade* t = new Trade("AAPL", 150.0);
    Trade* trades = new Trade[5]  {
        {"GOOGL", 2800.0},
        {"FB", 350.0},
        {"NFLX", 500.0},
        {"TSLA", 400.0},
        {"AMZN", 520.0}
    };
    delete t;
    delete[] trades;
    delete t;
    
    
    // code for PART 2
    Trade* t1 = new Trade("AAPL", 150.0);
    Trade* t2 = new Trade("GOOG", 2800.0);
    delete t1;
    //delete t1; // deletes t1 again, raising an error. We should delete t2 instead
    delete t2;

    // Trade* t3 = new Trade("MSFT", 300.0);
    // t3 = new Trade("TSLA", 750.0); // t3 now points to the TSLA trade, but the MSFT trade is still in memory and nothing points to it -> memory leak
    // we can fix this part using a unique pointer
    std::unique_ptr<Trade> t3 = std::make_unique<Trade>("MSFT", 300.0);
    t3 = std::make_unique<Trade>("TSLA", 750.0); // old one is automatically deleted

    Trade* trades = new Trade[3] {
        {"NVDA", 900.0},
        {"AMZN", 3200.0},
        {"META", 250.0}
    };
    //delete trades; // doesn't delete the whole array, just the first element. We have to use delet[]
    delete[] trades;
    

    // code for PART 3
    TradeHandle t1(new Trade("AAPL", 150.0));
    t1 = new Trade("MSFT", 250.0);

    return 0;
}
