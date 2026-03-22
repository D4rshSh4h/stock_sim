#ifndef SIMULATOR_H
#define SIMULATOR_H 

#include "orderbook.h"

class Simulator{
    public:
        Simulator();
        ~Simulator();
        float get_current_price() const;
        int get_volume(int time) const;
        void update_time();
        Orderbook& get_buy_book();
        Orderbook& get_sell_book();

    private:
        float current_price;
        int volume;
        int time;
        Orderbook buy_book;
        Orderbook sell_book;

};

#endif // SIMULATOR_H