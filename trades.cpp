//Handles logic for executing trades and updating the orderbook accordingly
#include "trades.h"
#include <iostream>

Order set_order(int price, int id, int type) {
    Order order(price, id, type);
    return order;
}

void match_orders(Order& order, Orderbook& buy, Orderbook& sell) {
    int trade_type = order.getTradeType(); // 0 for buy, 1 for sell
    int price = order.getPrice(); 
    
    //Buy order
    if(trade_type == 0){
        if(!sell.is_empty()){
            int lowest_sell_price = sell.find_lowest_price();
            if(lowest_sell_price == -1){
                std::cout << "Error: Sell orderbook is empty." << std::endl;
                return;
            }
            if (lowest_sell_price <= price){
                
            }

        }
    }
}