#ifndef AGENT_H
#define AGENT_H

#include "Simulator.h"
#include "../Order_structures/orderbook.h"
#include <random>

class Agent{
    private:
        Simulator& simulator;
        int id;
        float cash;
        int shares;
        char state; // b for buying, s for selling, h for holding, l for perfectly liquid, m or mix of both shares and cash
        std::optional<Order> prediction_engine();
        void place_order(Order& order);
        bool decide2();
        int decide3();
        static std::mt19937 gen;  // Random number generator for the prediction engine
        std::optional<Order> agent_buy(float mkt_price);
        std::optional<Order> agent_sell(float mkt_price);
    public:
        Agent() = default;
        Agent(Simulator& sim, int id, float cash, int shares, char state); 
        ~Agent();
        //current mkt price is a simulator function
        //Changing state will be done in log_trade in trades.cpp, unless order is bieng placed in which case it will be done here
        void run_agent();
        void change_state(char new_state, float added_cash, int added_shares);
        
    
}; 

#endif // AGENT_H