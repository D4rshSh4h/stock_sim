#include "Simulator.h" 
#include "Agent.h" 
#include "../config.h"
#include "rng.h"
#include <vector>
#include <algorithm>

Simulator::Simulator(float initial_price, float total_cash, int total_shares) : current_price(initial_price), volume(0), time(0), total_cash(total_cash), total_shares(total_shares) {}
Simulator::~Simulator() {}  
auto& gen = get_rng(); // Random number generator for the simulator

float Simulator::get_current_price() const {
    return current_price;
}
int Simulator::get_volume(int time) const {
    auto it = volume_time_log.find(time);
    if (it != volume_time_log.end()) {
        return it->second;
    }
    return 0; // Return 0 if no volume data is found for the given time
}
float Simulator::get_price(int time) const {
    auto it = price_time_log.find(time);
    if (it != price_time_log.end()) {
        return it->second;
    }
    return NO_BOOK_PRICE_FLOAT;
}

std::map<int, int> Simulator::get_volume_time_log() {
    return volume_time_log;
}

const std::map<int, int>& Simulator::get_volume_time_log_ref() const {
    return volume_time_log;
}

int Simulator::get_current_time() const {
    return time;
}

void Simulator::update_time() {time++;}

Orderbook& Simulator::get_buy_book() {
    return buy_book;
}   

Orderbook& Simulator::get_sell_book() {
    return sell_book;
}

void Simulator::reset_volume() {
    volume = 0;
}

void Simulator::log_price() { //TODO log into a file for price and volume over time, also log bid-ask spread over time
    price_time_log[time] = current_price;
}   

void Simulator::log_volume() {
    volume_time_log[time] = volume;
}

void Simulator::update_price(float lowest_ask, float highest_bid) {
    if (lowest_ask != NO_BOOK_PRICE_FLOAT && highest_bid != NO_BOOK_PRICE_FLOAT) {
        current_price = (lowest_ask + highest_bid) / 2.0; // Midpoint price
    } else if (lowest_ask != NO_BOOK_PRICE_FLOAT) {
        current_price = lowest_ask; // If only ask is available, use it
    } else if (highest_bid != NO_BOOK_PRICE_FLOAT) {
        current_price = highest_bid; // If only bid is available, use it
    }
}

Agent* Simulator::get_agent(int id) {
    auto it = agents.find(id);
    if (it != agents.end()) {
        return it->second.get();
    }
    return nullptr; // Return nullptr if no agent is found with the given id
}

void Simulator::simulator_buy_trade(Order& order) { //TODO logic for mid price/fair price
    matching_gateway.execute_buy(order, buy_book, sell_book, *this);
    /*
    float p = order.getPrice();
    if(p > best_bid){
        best_bid = p;
    }
    */
    
}

void Simulator::simulator_sell_trade(Order& order) {
    matching_gateway.execute_sell(order, buy_book, sell_book, *this);
    /*
    float p = order.getPrice();
    if(p < best_ask){
        best_ask = p;
    }
    */
    
} 

void Simulator::on_trade_executed(const Order& buy_order, const Order& sell_order, float price, float spread) {
    trade_settlement_service.settle_trade(
        buy_order, sell_order, price, spread, current_price, volume,
        [this](int id) { return get_agent(id); });
}

void Simulator::on_trade_agent_state(const Order& buy_order, const Order& sell_order, float price, float spread) {
    on_trade_executed(buy_order, sell_order, price, spread);
}

void Simulator::track_resting_order_for_timeout(std::shared_ptr<Order> order_ptr) {
    timeout_service.track_resting_order(order_ptr);
}

void Simulator::update_time_order_index(std::shared_ptr<Order> order_ptr) {
    track_resting_order_for_timeout(order_ptr);
}

void Simulator::expire_timed_out_orders(int timeout_duration) {
    timeout_service.expire_timed_out_orders(
        time, timeout_duration, [this](int id) { return get_agent(id); });
}

void Simulator::find_order_timeouts(int timeout_duration) {
    expire_timed_out_orders(timeout_duration);
}

void Simulator::create_vector_agent_ids() {
    agent_ids.clear();
    for (const auto& pair : agents) {
        agent_ids.push_back(pair.first);
    }
}

std::vector<int> Simulator::shuffle_agent_ids() {
    std::vector<int> shuffled_ids = agent_ids;
    //static std::mt19937 gen(std::random_device{}());
    std::shuffle(shuffled_ids.begin(), shuffled_ids.end(), gen);
    return shuffled_ids;
}

const IAgentDecisionEngine& Simulator::select_engine_for_agent(int agent_id) const {
    (void)agent_id;
    // Default path: all agents use the same engine. This is the extension point
    // for per-agent engine assignment inside initialize_agents/simulator_start.
    return random_decision_engine;
}

void Simulator::initialize_agents(int no_agents) {
    agent_bootstrapper.initialize_agents(
        *this, agents, agent_ids, no_agents, total_cash, total_shares, current_price,
        [this](int agent_id) -> const IAgentDecisionEngine& {
            return select_engine_for_agent(agent_id);
        });
}

void Simulator::simulator_start(int no_agents) {
    initialize_agents(no_agents);
}