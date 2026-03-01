#include "orderbook.h"
#include <iostream>

Orderbook::Orderbook() {}
Orderbook::~Orderbook() {}

void Orderbook::addOrder(Order order) {
    orderbook[order.getPrice()].push(order);
}
void Orderbook::removePrice(Order order) {
    int price_remove = order.getPrice();
    int check = orderbook.erase(price_remove);
    if (check == 0) {
        std::cout << "Order not found in orderbook." << std::endl;
    }

}

void Orderbook::removeOrder(Order order) {
    orderbook[order.getPrice()].pop();
}

bool Orderbook::is_empty() const {
    return orderbook.empty();
}

int Orderbook::find_lowest_price() const {
    if (orderbook.empty()) {
        return -1; // Return -1 or throw an exception if the orderbook is empty
    }
    return orderbook.begin()->first; // The lowest price is the first key in the map
}   

int Orderbook::find_highest_price() const {
    if (orderbook.empty()) {
        return -1; // Return -1 or throw an exception if the orderbook is empty
    }
    return orderbook.rbegin()->first; // The highest price is the last key in the map
}