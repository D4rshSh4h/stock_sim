//Handles logic for executing trades and updating the orderbook accordingly
#include "trades.h"
#include <iostream>
#include <fstream>

Order set_order(int price, int id, int type) {
    Order order(price, id, type);
    return order;
}

int calculate_spread(Order& buy_order, Order& sell_order) {
    return sell_order.getPrice() - buy_order.getPrice();
}

bool match_orders(Order& order, Orderbook& buy, Orderbook& sell) {
    int trade_type = order.getTradeType(); // 0 for buy, 1 for sell
    int price = order.getPrice(); 
    
    //order = a buy order
    if(trade_type == 0){
        while(!(sell.is_empty())){
            int lowest_sell_price = sell.find_lowest_price();
            if(lowest_sell_price == -1){
                std::cout << "Error: Sell orderbook is empty." << std::endl;
                return false;
            }
            if (lowest_sell_price <= price){
                auto sell_order = sell.get_order(lowest_sell_price);
                if (!sell_order.has_value()) {
                    std::cout << "Error: No sell orders found at lowest price." << std::endl;
                    return false;
                }
                std::cout << "Trade executed at price: " << lowest_sell_price << std::endl;
                int spread = calculate_spread(order, *sell_order);
                log_trade(order, *sell_order, lowest_sell_price, spread);
                sell.removeOrder(*sell_order);
            }

        }
    }

    if(trade_type == 1){
        while(!(buy.is_empty())){
            int highest_buy_price = buy.find_highest_price();
            if(highest_buy_price == -1){
                std::cout << "Error: Buy orderbook is empty." << std::endl;
                return false;
            }
            if (highest_buy_price >= price){
                auto buy_order = buy.get_order(highest_buy_price);
                if (!buy_order.has_value()) {
                    std::cout << "Error: No buy orders found at highest price." << std::endl;
                    return false;
                }
                std::cout << "Trade executed at price: " << highest_buy_price << std::endl;
                int spread = calculate_spread(*buy_order, order);
                log_trade(order, *buy_order, highest_buy_price, spread);
                buy.removeOrder(*buy_order);
            }

        }
    }
    return true;
}

//Receives a new incoming order, Decides what to do with it, Calls the matching logic, If unmatched, rests the order in the book
bool buy_trade(Order& order, Orderbook& buy, Orderbook& sell) {
    //If trade not matched add to orderbook
    if (!match_orders(order, buy, sell)) {
        buy.addOrder(order);
        return false;
    }
}

bool sell_trade(Order& order, Orderbook& buy, Orderbook& sell) {
    //If trade not matched add to orderbook
    if (!match_orders(order, buy, sell)) {
        sell.addOrder(order);
        return false;
    }
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

void log_trade(Order& buyer, Order& seller, int price, int spread) {
    prepare_file();
    std::ofstream log_file("trade_log.csv", std::ios::app);
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