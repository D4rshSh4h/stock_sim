//Handles logic for executing trades and updating the orderbook accordingly
#include "trades.h"
#include "../Simulator_structures/TradeObserver.h"
#include <iostream>
#include <fstream>

TradeObserver* trade_observer = nullptr; // Global pointer to the trade observer
void set_trade_observer(TradeObserver* observer) {
    trade_observer = observer;
}

Order set_order(float price, int id, int type) {
    Order order(price, id, type);
    return order;
}

float calculate_spread(Order& buy_order, Order& sell_order) {
    return sell_order.getPrice() - buy_order.getPrice();
}

// Matches a single incoming order against the opposite book
bool match_orders(Order& order, Orderbook& buy, Orderbook& sell) {
    int trade_type = order.getTradeType();
    float price = order.getPrice(); 
    bool matched = false;
    
    if(trade_type == 0){
        while(!(sell.is_empty())){
            float lowest_sell_price = sell.find_lowest_price();
            if(lowest_sell_price == -1){
                return false;
            }
            if (lowest_sell_price <= price){
                auto sell_order = sell.get_order(lowest_sell_price);
                if (!sell_order.has_value()){
                    return false;
                }
                float spread = calculate_spread(order, *sell_order);
                log_trade(order, *sell_order, lowest_sell_price, spread);
                sell.removeOrder(*sell_order);
                matched = true;
                break; // incoming order fully filled, stop
            } else {
                break;
            }
        }
    }

    if(trade_type == 1){
        while(!(buy.is_empty())){
            float highest_buy_price = buy.find_highest_price();
            if(highest_buy_price == -1){
                return false;
            }
            if (highest_buy_price >= price){
                auto buy_order = buy.get_order(highest_buy_price);
                if (!buy_order.has_value()){
                    return false;
                }
                float spread = calculate_spread(*buy_order, order);
                log_trade(*buy_order, order, highest_buy_price, spread);
                buy.removeOrder(*buy_order);
                matched = true;
                break; // incoming order fully filled, stop
            } else {
                break;
            }
        }
    }
    return matched;
}

// Sweeps resting orders in both books and matches them against each other
void sweep_book(Orderbook& buy, Orderbook& sell) {
    while (!buy.is_empty() && !sell.is_empty()) {
        float highest_buy_price = buy.find_highest_price();
        float lowest_sell_price = sell.find_lowest_price();

        if (highest_buy_price == -1 || lowest_sell_price == -1) break;

        if (highest_buy_price >= lowest_sell_price) {
            auto buy_order = buy.get_order(highest_buy_price);
            auto sell_order = sell.get_order(lowest_sell_price);

            if (!buy_order.has_value() || !sell_order.has_value()) break;

            float spread = calculate_spread(*buy_order, *sell_order);
            log_trade(*buy_order, *sell_order, lowest_sell_price, spread);
            buy.removeOrder(*buy_order);
            sell.removeOrder(*sell_order);
            // remove break here when handling multiple trades
        } else {
            break;
        }
    }
}

//Receives a new incoming order, Decides what to do with it, Calls the matching logic, If unmatched, rests the order in the book
bool buy_trade(Order& order, Orderbook& buy, Orderbook& sell) { //TODO add param for callback
    //If trade not matched add to orderbook
    if (!match_orders(order, buy, sell)) {
        buy.addOrder(order);
        return false;
    }
    return true;
}

bool sell_trade(Order& order, Orderbook& buy, Orderbook& sell) {
    //If trade not matched add to orderbook
    if (!match_orders(order, buy, sell)) {
        sell.addOrder(order);
        return false;
    }
    return true;
}

void prepare_file(void) { 
    std::ofstream log_file("trade_log.csv", std::ios::trunc);
    if (log_file.is_open()) {
        log_file << "Buyer ID, Seller ID, Price, Spread" << std::endl;
        log_file.close();
    } else {
        std::cout << "Unable to initialize trade log file." << std::endl;
    }
}

void log_trade(Order& buyer, Order& seller, float price, float spread) {
    prepare_file(); //TODO check logic
    //TODO: logic to update state of agents + update thier cash/shares based on the trade. (cash+shares already deducted when order placed)
    if(trade_observer){
        trade_observer->on_trade_agent_state(buyer, seller, price, spread);
    }
    else{
        std::cout << "No trade observer set. Unable to update agent states." << std::endl;
    }
    std::ofstream log_file("trade_log.csv",      std::ios::app);
    if (log_file.is_open()) {
        log_file << buyer.getId() 
                 << ", " << seller.getId() 
                 << ", " << price 
                 << ", " << spread 
                 << std::endl;
        log_file.close();
    } else {
        std::cout << "Unable to open trade log file." << std::endl;
    }
}

