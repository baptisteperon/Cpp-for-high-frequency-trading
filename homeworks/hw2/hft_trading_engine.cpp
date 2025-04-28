#include <vector>
#include <span>
#include <random>
#include <chrono>
#include <iostream>
#include <unordered_map>

using namespace std;

struct alignas(64) MarketData {
    int instrument_id;
    double price;
    std::chrono::high_resolution_clock::time_point timestamp;
};


struct alignas(64) Order {
    int instrument_id;
    double price;
    bool is_buy;
    std::chrono::high_resolution_clock::time_point timestamp;
};


class MarketDataFeed {
    public:
        MarketDataFeed(std::vector<MarketData>& ref) : data(ref) {}
    
        void generateData(int num_ticks) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> price_dist(100.0, 200.0);
    
            data.reserve(num_ticks);
            for (int i = 0; i < num_ticks; ++i) {
                MarketData md;
                md.instrument_id = i % 10;
                md.price = price_dist(gen);
                md.timestamp = std::chrono::high_resolution_clock::now();
                data.push_back(md);
            }
        }
    
    private:
        std::vector<MarketData>& data;
};

class TradeEngine {
    public:
        TradeEngine(const std::vector<MarketData>& feed)
            : market_data(feed) {
                strat_trigger_counter[1] = 0;
                strat_trigger_counter[2] = 0;
                strat_trigger_counter[3] = 0;
                strat_trigger_counter[4] = 0;
            }
    
        void process() {
            int strat_triggered;
            for (const auto& tick : market_data) {
                // Update history
                updateHistory(tick);
    
                // Apply signals
                bool buy = false, sell = false;

                strat_triggered = 0;
    
                if (signal1(tick)) {buy = true; strat_triggered=1;};
                if (signal2(tick)) { if (tick.price < getAvg(tick.instrument_id)) buy = true; else sell = true; strat_triggered=2;}
                if (signal3(tick)) {buy = true; strat_triggered=3;};
                if (signal4(tick, 1.1)) {buy = true;  strat_triggered=4;};
    
                if (buy || sell) {
                    auto now = std::chrono::high_resolution_clock::now();
                    Order o { tick.instrument_id, tick.price + (buy ? 0.01 : -0.01), buy, now };
                    orders.push_back(o);
    
                    auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(now - tick.timestamp).count();
                    latencies.push_back(latency);

                    strat_trigger_counter[strat_triggered]++;
                }
            }
        }
    
        void reportStats() {
            long long sum = 0, max_latency = 0;
            for (auto l : latencies) {
                sum += l;
                if (l > max_latency) max_latency = l;
            }
    
            std::cout << "\n--- Performance Report ---\n";
            std::cout << "Total Market Ticks Processed: " << market_data.size() << "\n";
            std::cout << "Total Orders Placed: " << orders.size() << "\n";
            std::cout << "Average Tick-to-Trade Latency (ns): " << (latencies.empty() ? 0 : sum / latencies.size()) << "\n";
            std::cout << "Maximum Tick-to-Trade Latency (ns): " << max_latency << "\n";
            std::cout << "Price threholds strategy has produced: " << strat_trigger_counter[1] << " orders\n";
            std::cout << "Deviation from average strategy has produced: " << strat_trigger_counter[2] << " orders\n";
            std::cout << "Simple momentum strategy has produced: " << strat_trigger_counter[3] << " orders\n";
            std::cout << "Volatility based strategy has produced: " << strat_trigger_counter[4] << " orders\n";
        }
    
    private:
        const std::vector<MarketData>& market_data;
        std::vector<Order> orders;
        std::vector<long long> latencies;
        std::unordered_map<int, std::vector<double>> price_history;
        std::unordered_map<int, int> strat_trigger_counter;
    
        void updateHistory(const MarketData& tick) {
            auto& hist = price_history[tick.instrument_id];
            hist.push_back(tick.price);
            if (hist.size() > 10) hist.erase(hist.begin());
        }
    
        double getAvg(int id) {
            auto& hist = price_history[id];
            double sum = 0;
            for (double p : hist) sum += p;
            return hist.empty() ? 0 : sum / hist.size();
        }

        double getVol(int id, int windowsize) {
            const auto& full_hist = price_history[id];
            if (full_hist.size() < windowsize) return 0;

            std::vector<double> recent_hist(full_hist.end() - windowsize, full_hist.end());
            double sum = 0;
            for (double p : recent_hist) sum += p;
            double avg = sum / recent_hist.size();

            double var = 0;
            for (double p : recent_hist) var += (p - avg) * (p - avg);
            return std::sqrt(var / recent_hist.size());
        }

        // Signal 1: Price thresholds
        bool signal1(const MarketData& tick) {
            return tick.price < 105.0 || tick.price > 195.0;
        }
    
        // Signal 2: Deviation from average
        bool signal2(const MarketData& tick) {
            if (price_history[tick.instrument_id].size() < 5) return false;
            double avg = getAvg(tick.instrument_id);
            return tick.price < avg * 0.98 || tick.price > avg * 1.02;
        }
    
        // Signal 3: Simple momentum
        bool signal3(const MarketData& tick) {
            auto& hist = price_history[tick.instrument_id];
            if (hist.size() < 3) return false;
            double diff1 = hist[hist.size() - 2] - hist[hist.size() - 3];
            double diff2 = hist[hist.size() - 1] - hist[hist.size() - 2];
            return diff1 > 0 && diff2 > 0;
        }

        // Signal 4: Volatility based strategy
        bool signal4(const MarketData& tick, double threshold) {
            if (price_history[tick.instrument_id].size() < 10) return false;
            double vol_5 = getVol(tick.instrument_id, 5); // vol for the past 5 ticks
            double vol_10 = getVol(tick.instrument_id, 10); // vol for the past 10 ticks
            return vol_5 > vol_10*threshold; // buy if volatility has increased more than a given threshold
        }
};

int main() {
    std::vector<MarketData> feed;
    MarketDataFeed generator(feed);

    auto start = std::chrono::high_resolution_clock::now();
    generator.generateData(100000);

    TradeEngine engine(feed);
    engine.process();

    auto end = std::chrono::high_resolution_clock::now();
    auto runtime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    engine.reportStats();
    std::cout << "Total Runtime (ms): " << runtime << std::endl;

    return 0;
}