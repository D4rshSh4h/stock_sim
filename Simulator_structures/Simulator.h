#ifndef SIMULATOR_H
#define SIMULATOR_H 


#include "../Order_structures/orderbook.h"
#include "TradeObserver.h"
#include <unordered_map>
#include <memory>

class Agent;
 
class Simulator : public TradeObserver {
    public:
        Simulator(float initial_price = 100.0);
        Simulator() = default;
        ~Simulator();
        float get_current_price() const;
        int get_volume(int time) const; //Gets a single volume
        float get_price(int time) const; //Gets a single price
        // TODO: Implement volume by time and price by time functions
        void update_time(); 
        void log_price(); //Adds current price to price_time_log with current time as key
        void log_volume(); //Adds current volume to volume_time_log with current time as key
        void update_price(float lowest_ask, float highest_bid);
        Orderbook& get_buy_book();
        Orderbook& get_sell_book();
        //TODO - function to generate a certain number of agents with random cash/shares and add to agents map. 
        Agent* get_agent(int id);
        void simulator_buy_trade(Order& order);
        void simulator_sell_trade(Order& order); 
        void on_trade_agent_state(const Order& buy_order, const Order& sell_order, float price, float spread) override; //TODO - implement logic to update state of agents + update thier cash/shares based on the trade. (cash+shares already deducted when order placed)
    private:
        float current_price;
        int volume;
        int time;
        Orderbook buy_book;
        Orderbook sell_book;
        std::map<int, int> volume_time_log; // maps time to volume for that time step <int time, int volume>
        std::map<int, float> price_time_log; // maps time to price for that time step <int time, float price>
        std::unordered_map<int, std::unique_ptr<Agent>> agents; // maps agent id to agent object <int id, Agent agent>

};

#endif // SIMULATOR_H