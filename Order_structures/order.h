#ifndef ORDER_H
#define ORDER_H

#include "../config.h"

class Order {
private:
    int price; // Integer representation of price via config multiplier
    int id; //ID of agent which placed the order
    OrderType trade_type; 
    int time_placed;
    OrderStatus state; 

public:
    Order(int price, int id, OrderType trade_type, int time_placed, OrderStatus state);
    Order() = default;

    int getPrice() const;
    int getId() const;
    OrderType getTradeType() const;
    int getTimePlaced() const;
    OrderStatus getStatus() const;

    void fill_order();
    void timeout_order();
    void cancel_order();
};

#endif // ORDER_H