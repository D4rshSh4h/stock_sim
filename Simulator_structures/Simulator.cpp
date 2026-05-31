#include "Simulator.h" 
#include "Agent.h" 
#include "../Trade_logic/trades.h"
#include "../config.h"
#include "rng.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <iostream>

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

void Simulator::log_price() { 
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

void Simulator::simulator_buy_trade(Order& order) { 
    buy_trade(order, buy_book, sell_book, *this);
    /*
    float p = order.getPrice();
    if(p > best_bid){
        best_bid = p;
    }
    */
    
}

void Simulator::simulator_sell_trade(Order& order) {
    sell_trade(order, buy_book, sell_book, *this);
    /*
    float p = order.getPrice();
    if(p < best_ask){
        best_ask = p;
    }
    */
    
} 



void Simulator::on_trade_agent_state(const Order& buy_order, const Order& sell_order, float price, float spread, int qty) {
    //qty refers to the number of shares traded in this particular trade
    int buy_id = buy_order.getId();
    int sell_id = sell_order.getId();

    if(price == 0.00){
        current_price = price + BUFFER;
    }
    else{
        current_price = price;
    }

    Agent* buyer = get_agent(buy_id);
    Agent* seller = get_agent(sell_id);

    if (buyer) {
        buyer->change_state(AgentState::Holding, spread*qty, qty); // Buyer receives qty shares + changed state to holding
    }
    if (seller) {
        seller->change_state(AgentState::Liquid, price*qty, 0); // Seller receives cash + changed state to perfectly liquid
    }
    volume += qty;
}



void Simulator::update_time_order_index(std::shared_ptr<Order> order_ptr) {
    //track_resting_order_for_timeout(order_ptr);
    if (order_ptr) {
        timeout_queue.push(order_ptr);
    }
}
 


void Simulator::find_order_timeouts(int timeout_duration) {
    //expire_timed_out_orders(timeout_duration);
    while (!timeout_queue.empty()) {
        std::shared_ptr<Order> order = timeout_queue.front().lock();
        if (!order) {
            timeout_queue.pop();
            continue;
        }
        if (order->getStatus() != OrderStatus::Active) {
            timeout_queue.pop();
            continue;
        }
        if (time - order->getTimePlaced() < timeout_duration) {
            break;
        }

        order->timeout_order();
        order->cancel_order();
        Agent* agent_change = get_agent(order->getId());
        if (agent_change) {
            int order_qty = order->getQty(); //Qty of shares of order in orderbook remaining
            if(order->getTradeType() == OrderType::Buy){
                agent_change->change_state(agent_change->get_state(), int_to_float_price(order->getPrice())*order_qty, 0);
            } else {
                agent_change->change_state(agent_change->get_state(), 0, order_qty);
            }
        }
        timeout_queue.pop();
    }
}

//Agent initialization

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

size_t Simulator::choose_split_index(int no_agents) {
    std::uniform_int_distribution<size_t> dist(1, no_agents - 1);
    return dist(gen);
}

const IAgentDecisionEngine& Simulator::select_engine_for_agent(int agent_id) const {
    (void)agent_id;
    // Default path: all agents use the same engine. This is the extension point
    // for per-agent engine assignment inside initialize_agents/simulator_start.
    return random_decision_engine;
}

bool Simulator::allocate_cash_agents(std::vector<std::unique_ptr<Agent>>& temp_agent_vector, int no_cash_agents) {
    float cuts_cash = total_cash - (no_cash_agents * current_price);
    if(cuts_cash < 0){
        std::cout << "Not enough total cash to give each cash agent enough to buy 1 share at initial price. Please adjust total cash or number of agents." << std::endl;
        return false;
    }
    std::vector<float> cut_points_cash;
    cut_points_cash.reserve(no_cash_agents - 1);
    std::uniform_real_distribution<float> cut_dist(0, cuts_cash);
    for(int i = 0; i< no_cash_agents-1; i++){
        cut_points_cash.push_back(cut_dist(gen));
    }
    std::sort(cut_points_cash.begin(), cut_points_cash.end());
    for(int i = 0; i< no_cash_agents; i++){
        if(i == 0){
            temp_agent_vector[i]->change_state(AgentState::Buying, cut_points_cash[i] + current_price, 0);
        }
        else if(i == no_cash_agents-1){
            temp_agent_vector[i]->change_state(AgentState::Buying, cuts_cash - cut_points_cash[i-1] + current_price, 0);
        }
        else{
            temp_agent_vector[i]->change_state(AgentState::Buying, cut_points_cash[i] - cut_points_cash[i-1] + current_price, 0);
        }
    }
    return true;
}

void Simulator::allocate_share_agents(std::vector<std::unique_ptr<Agent>>& temp_agent_vector, int no_agents, size_t split_index) {
    int no_share_agents = no_agents - static_cast<int>(split_index);
    int cuts_shares = total_shares - no_share_agents;
    std::vector<int> cut_points_shares;
    cut_points_shares.reserve(no_share_agents - 1);
    std::vector<int> positions(cuts_shares);
    std::iota(positions.begin(), positions.end(), 1);
    std::sample(positions.begin(), positions.end(), std::back_inserter(cut_points_shares), no_share_agents - 1, gen);
    std::sort(cut_points_shares.begin(), cut_points_shares.end());
    for(int i = static_cast<int>(split_index); i< no_agents; i++){
        if(static_cast<size_t>(i) == split_index){
            temp_agent_vector[i]->change_state(AgentState::Selling, 0, cut_points_shares[i - static_cast<int>(split_index)] + 1);
        }
        else if(i == no_agents-1){
            temp_agent_vector[i]->change_state(AgentState::Selling, 0, cuts_shares - cut_points_shares[i - static_cast<int>(split_index) - 1] + 1);
        }
        else{
            temp_agent_vector[i]->change_state(AgentState::Selling, 0, cut_points_shares[i - static_cast<int>(split_index)] - cut_points_shares[i - static_cast<int>(split_index) - 1] + 1);
        }
    }
}

void Simulator::register_agents(std::vector<std::unique_ptr<Agent>>& temp_agent_vector) {
    for(auto& agent_ptr : temp_agent_vector){
        int id = agent_ptr->get_id();
        agents[id] = std::move(agent_ptr);
    }
    create_vector_agent_ids();
}

void Simulator::initialize_agents(int no_agents) {
    std::vector<std::unique_ptr<Agent>> temp_agent_vector;
    for(int i = 0; i< no_agents; i++){
        const IAgentDecisionEngine& engine = select_engine_for_agent(i);
        temp_agent_vector.emplace_back(std::make_unique<Agent>(*this, engine, i, 0.00, 0, AgentState::Uninitialized));
    }
   
    std::shuffle(temp_agent_vector.begin(), temp_agent_vector.end(), gen);
    size_t split_index = choose_split_index(no_agents);
    int no_cash_agents = static_cast<int>(split_index);
    if (!allocate_cash_agents(temp_agent_vector, no_cash_agents)) {
        return;
    }
    allocate_share_agents(temp_agent_vector, no_agents, split_index);
    register_agents(temp_agent_vector);
}

void Simulator::simulator_start(int no_agents) {
    initialize_agents(no_agents);
}