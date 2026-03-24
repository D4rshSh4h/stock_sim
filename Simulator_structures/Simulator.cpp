#include "Simulator.h"  

Simulator::Simulator(float initial_price) : current_price(initial_price), volume(0), time(0) {}
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