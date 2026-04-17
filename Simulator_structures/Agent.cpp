#include "Agent.h"
#include "rng.h"
#include <iostream>
//#include <stdexcept>

const float RANGE = 0.05; //Range for random price generation in prediction engine

Agent::Agent(Simulator& sim, int id, float cash, int shares, char state) : simulator(sim), id(id), cash(cash), shares(shares), state(state) {}
Agent::~Agent() {}

//std::mt19937 Agent::gen{std::random_device{}()};   Random number generator for the prediction engine
bool Agent::decide2() {
    std::bernoulli_distribution dist(0.5); // 50% chance to return true or false
    return dist(get_rng()); // Returns true or false
}
int Agent::decide3() {
    std::uniform_int_distribution<> dist(0, 2); // 0, 1, or 2
    return dist(get_rng()); // Returns 0, 1, or 2

}

std::optional<Order> Agent::agent_buy(float mkt_price) {
    float delta = RANGE * mkt_price;
    std::uniform_real_distribution<float> price_dist(mkt_price - delta, mkt_price + delta);
    float price = price_dist(get_rng());
    if (cash < price){
        return std::nullopt; // Not enough cash to buy
    }
    return std::make_optional(Order(price, id, 0, simulator.get_current_time(), 0));
}

std::optional<Order> Agent::agent_sell(float mkt_price) {
    float delta = RANGE * mkt_price;
    std::uniform_real_distribution<float> price_dist(mkt_price - delta, mkt_price + delta);
    float price = price_dist(get_rng());
    if(shares < 1){
        return std::nullopt; // Not enough shares to sell
    }
    return std::make_optional(Order(price, id, 1, simulator.get_current_time(), 0));
}

std::optional<Order> Agent::prediction_engine() {
    float current_price = simulator.get_current_price();
    /*
    This first iteration is a completely random prediction engine. Therefore, the state of the agent will not be used.
    Instead the agent will randomly decide to buy, sell or hold, given the constraints of its cash and shares. 
    Reminder that currently only one share can be traded at a time. 
    0 cash and 0 shares will throw an error (TODO: implement logic to remove agent from sim)
    An agent which has an order placed in an orderbook will have its cash/shares deducted until said order is filled.
    */

    if(cash == 0 && shares == 0){
        throw std::runtime_error("Agent has no cash or shares to trade");
    }
    else if(shares == 0){
        //Only have cash - random chance to buy or do nothing
        bool decision = decide2();
        if(decision){
            //Buy
            return agent_buy(current_price);
        }
        else{
            //Do nothing
            return std::nullopt;
        }
    }
    else if(cash == 0){
        //Only have shares - random chance to sell or do nothing
        bool decision = decide2();
        if(decision){
            //Sell
            return agent_sell(current_price);
        }
        else{
            //Do nothing
            return std::nullopt;
        }
    }
    else{
        //Have both cash and shares - random chance to buy, sell or do nothing
        int decision = decide3();
        if(decision == 0){  
            //Buy
            return agent_buy(current_price);
        }
        else if(decision == 1){
            //Sell
            return agent_sell(current_price);
        }
        else{
            //Do nothing
            return std::nullopt;
        }
    }

}

void Agent::place_order(Order& order) {
    if (order.getTradeType() == 0) {
        simulator.simulator_buy_trade(order); 
        cash -= order.getPrice(); // Deduct cash immediately when placing a buy order
    } else {
        simulator.simulator_sell_trade(order); 
        shares -= 1; // Deduct shares immediately when placing a sell order (assuming 1 share per order)
    }
} 

//TODO: need to create a function where an order can time out
void Agent::run_agent() {
    auto order_opt = prediction_engine();
    if (order_opt.has_value()) {
        place_order(*order_opt);
        if(order_opt->getTradeType() == 0){
            state = 'b';
        }
        else{
            state = 's';
        }
    }
}

void Agent::change_state(char new_state, float added_cash, int added_shares) {
    state = new_state;
    cash += added_cash;
    shares += added_shares;
}

int Agent::get_id() const {
    return id;
}

char Agent::get_state() const {
    return state;
}