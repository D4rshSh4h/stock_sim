#include "orderbook.h"
#include <iostream>

Orderbook::Orderbook() {}
Orderbook::~Orderbook() {}

void Orderbook::addOrder(Order order) {
    orderbook[order.getPrice()].push(order);
}
void Orderbook::removeOrder(Order order) {
    int price_remove = order.getPrice();
    int check = orderbook.erase(price_remove);
    if (check == 0) {
        std::cout << "Order not found in orderbook." << std::endl;
    }

}
