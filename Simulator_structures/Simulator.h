#ifndef SIMULATOR_H
#define SIMULATOR_H 


#include "../Order_structures/orderbook.h"

class Simulator{
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

    private:
        float current_price;
        int volume;
        int time;
        Orderbook buy_book;
        Orderbook sell_book;
        std::map<int, int> volume_time_log; // maps time to volume for that time step <int time, int volume>
        std::map<int, float> price_time_log; // maps time to price for that time step <int time, float price>

};

#endif // SIMULATOR_H