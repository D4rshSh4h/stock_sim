#include "Order.h"

Order::Order(int price, int id)
    : price(price), id(id) {}

int Order::getPrice() const {
    return price;
}

int Order::getId() const {
    return id;
}