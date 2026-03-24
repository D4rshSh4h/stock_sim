#include "order.h"

Order::Order(int price, int id, int trade_type)
    : price(price), id(id), trade_type(trade_type) {}

float Order::getPrice() const { return price; }

int Order::getTradeType() const { return trade_type; }

int Order::getId() const { return id; }