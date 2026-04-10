#include "Simulator.h" 
#include "Agent.h" 
#include "../Trade_logic/trades.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <iostream>

Simulator::Simulator(float initial_price, float total_cash, int total_shares) : current_price(initial_price), volume(0), time(0), total_cash(total_cash), total_shares(total_shares) {}
Simulator::~Simulator() {}  

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
    return -1.0; // Return -1 or throw an exception if no price data is found for the given time
}

void Simulator::update_time() {time++;}

Orderbook& Simulator::get_buy_book() {
    return buy_book;
}   

Orderbook& Simulator::get_sell_book() {
    return sell_book;
}

void Simulator::log_price() {
    price_time_log[time] = current_price;
}   

void Simulator::log_volume() {
    volume_time_log[time] = volume;
}

void Simulator::update_price(float lowest_ask, float highest_bid) {
    if (lowest_ask != -1 && highest_bid != -1) {
        current_price = (lowest_ask + highest_bid) / 2.0; // Midpoint price
    } else if (lowest_ask != -1) {
        current_price = lowest_ask; // If only ask is available, use it
    } else if (highest_bid != -1) {
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
    buy_trade(order, buy_book, sell_book);
}

void Simulator::simulator_sell_trade(Order& order) {
    sell_trade(order, sell_book, buy_book);
} 

void Simulator::on_trade_agent_state(const Order& buy_order, const Order& sell_order, float price, float spread) {
    
    int buy_id = buy_order.getId();
    int sell_id = sell_order.getId();

    Agent* buyer = get_agent(buy_id);
    Agent* seller = get_agent(sell_id);

    if (buyer) {
        buyer->change_state('h', 0, 1); // Buyer receives 1 share + changed state to holding
    }
    if (seller) {
        seller->change_state('l', price, 0); // Seller receives cash + changed state to perfectly liquid
    }
}

void Simulator::simulator_start(int no_agents) {
    std::vector<std::unique_ptr<Agent>> temp_agent_vector;
    for(int i = 0; i< no_agents; i++){
        temp_agent_vector.emplace_back(std::make_unique<Agent>(*this, i, 0.00, 0, 'u'));
    }
   
    std::mt19937 gen(std::random_device{}());
    std::shuffle(temp_agent_vector.begin(), temp_agent_vector.end(), gen);
    std::uniform_int_distribution<size_t> dist(1, no_agents-1);
    size_t split_index = dist(gen); //Use this for the two groups of agents, one with cash and one with shares.

    //Cash
    //Each agent must have at least current_price (initial price) amount of cash
    int no_cash_agents = split_index; 
    float cuts_cash = total_cash - (no_cash_agents * current_price);
    if(cuts_cash < 0){
        std::cout << "Not enough total cash to give each cash agent enough to buy 1 share at initial price. Please adjust total cash or number of agents." << std::endl;
        return;
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
            temp_agent_vector[i]->change_state('b', cut_points_cash[i] + current_price, 0); //Give first cash agent a random cut of the remaining cash + enough to buy 1 share at initial price
        }
        else if(i == no_cash_agents-1){
            temp_agent_vector[i]->change_state('b', cuts_cash - cut_points_cash[i-1] + current_price, 0); //Give last cash agent the rest of the remaining cash + enough to buy 1 share at initial price
        }
        else{
            temp_agent_vector[i]->change_state('b', cut_points_cash[i] - cut_points_cash[i-1] + current_price, 0); //Give each other cash agent a random cut of the remaining cash + enough to buy 1 share at initial price
        }
    }

    //Shares
    //Each agent must have at least 1 share
    int no_share_agents = no_agents - split_index;
    int cuts_shares = total_shares - no_share_agents;
    std::vector<int> cut_points_shares;
    cut_points_shares.reserve(no_share_agents - 1);
    std::vector<int> positions(cuts_shares);
    std::iota(positions.begin(), positions.end(), 1); 
    std::sample(positions.begin(), positions.end(), std::back_inserter(cut_points_shares), no_share_agents - 1, gen);
    std::sort(cut_points_shares.begin(), cut_points_shares.end());
    for(int i = split_index; i< no_agents; i++){
        if(i == split_index){
            temp_agent_vector[i]->change_state('s', 0, cut_points_shares[i - split_index] + 1); //Give first share agent a random cut of the remaining shares + 1 share
        }
        else if(i == no_agents-1){
            temp_agent_vector[i]->change_state('s', 0, cuts_shares - cut_points_shares[i - split_index - 1] + 1); //Give last share agent the rest of the remaining shares + 1 share
        }
        else{
            temp_agent_vector[i]->change_state('s', 0, cut_points_shares[i - split_index] - cut_points_shares[i - split_index - 1] + 1); //Give each other share agent a random cut of the remaining shares + 1 share
        }
    }
    for(auto& agent_ptr : temp_agent_vector){
    int id = agent_ptr->get_id();
    agents[id] = std::move(agent_ptr);
}
    

}