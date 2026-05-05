#include "order.h"

Order::Order(int price, int id, OrderType trade_type, int time_placed, OrderStatus state, int qty)
    : price(price), id(id), trade_type(trade_type), time_placed(time_placed), state(state), qty(qty) {}

int Order::getPrice() const { return price; }

OrderType Order::getTradeType() const { return trade_type; }

OrderStatus Order::getStatus() const { return state; }

int Order::getTimePlaced() const { return time_placed; }

int Order::getId() const { return id; } 

int Order::getQty() const { return qty; }

void Order::changeQty(int added_qty) {
    qty += added_qty;
}  

void Order::fill_order() {
    state = OrderStatus::Filled;
}

void Order::timeout_order() {
    state = OrderStatus::Timeouted; 
}

void Order::cancel_order() {
    state = OrderStatus::Cancelled;
}