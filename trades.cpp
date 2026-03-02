//Handles logic for executing trades and updating the orderbook accordingly
#include "trades.h"
#include <iostream>

Order set_order(int price, int id, int type) {
    Order order(price, id, type);
    return order;
}
//TODO correct using claude's feedback + add spread cost
void match_orders(Order& order, Orderbook& buy, Orderbook& sell) {
    int trade_type = order.getTradeType(); // 0 for buy, 1 for sell
    int price = order.getPrice(); 
    
    //order = a buy order
    if(trade_type == 0){
        if(!(sell.is_empty())){
            int lowest_sell_price = sell.find_lowest_price();
            if(lowest_sell_price == -1){
                std::cout << "Error: Sell orderbook is empty." << std::endl;
                return;
            }
            if (lowest_sell_price <= price){
                auto result = sell.get_order(price);
                if (!result.has_value()) {
                    std::cout << "Error: No sell orders found at this price." << std::endl;
                    return;
                }
                auto sell_order = *result;
                auto book_price = sell_order.getPrice();
                log_trade(order, sell_order, book_price);
                sell.removeOrder(sell_order);
            }

        }
    }

    if(trade_type == 1){
        if(!(buy.is_empty())){
            int highest_buy_price = buy.find_highest_price();
            if(highest_buy_price == -1){
                std::cout << "Error: Buy orderbook is empty." << std::endl;
                return;
            }
            if (highest_buy_price >= price){
                auto result = buy.get_order(price);
                if (!result.has_value()) {
                    std::cout << "Error: No buy orders found at this price." << std::endl;
                    return;
                }
                auto buy_order = *result;
                auto book_price = buy_order.getPrice();
                log_trade(buy_order, order, book_price);
                buy.removeOrder(buy_order);
            }

        }
    }
}