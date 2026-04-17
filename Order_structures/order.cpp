#include "order.h"

Order::Order(int price, int id, int trade_type, int time_placed, int state)
    : price(price), id(id), trade_type(trade_type), time_placed(time_placed), state(state) {}

float Order::getPrice() const { return price; }

int Order::getTradeType() const { return trade_type; }

int Order::getStatus() const { return state; }

int Order::getTimePlaced() const { return time_placed; }

int Order::getId() const { return id; } 

void Order::timeout_order() {
    state = 1; // Set state to timeouted
}