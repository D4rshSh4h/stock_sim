#include "orderbook.h"
#include <iostream>

Orderbook::Orderbook() {}
Orderbook::~Orderbook() {}

void Orderbook::addOrder(const Order &order) {
    orderbook[order.getPrice()].push_back(order);
}
void Orderbook::removePrice(const Order &order) {
    float price_remove = order.getPrice();
    int check = orderbook.erase(price_remove);
    if (check == 0) {
        std::cout << "Order not found in orderbook." << std::endl;
    }

}

//Removes the order at the front of the queue for the given price. If the queue is empty, removes the price from the orderbook
void Orderbook::removeOrder(const Order &order) {
    float price_index = order.getPrice();
    if(!orderbook[price_index].empty()){
        orderbook[price_index].pop_front();
        if(orderbook[price_index].empty()){
            this -> removePrice(order);
        }
    }
    else{
        this -> removePrice(order);
    }
    
}

bool Orderbook::is_empty() const {
    return orderbook.empty();
}

float Orderbook::find_lowest_price() const {
    if (orderbook.empty()) {
        return -1; // Return -1 or throw an exception if the orderbook is empty
    }
    return orderbook.begin()->first; // The lowest price is the first key in the map
}   

float Orderbook::find_highest_price() const {
    if (orderbook.empty()) {
        return -1; // Return -1 or throw an exception if the orderbook is empty
    }
    return orderbook.rbegin()->first; // The highest price is the last key in the map
}

std::optional<Order> Orderbook::get_order(float price) {
    auto it = orderbook.find(price);
    if(it == orderbook.end() || it->second.empty()){
        return std::nullopt; // Return an empty optional if no orders are found at this price
    }
    else{
        Order order_collected = it->second.front();
        if(order_collected.getStatus() == 0){
            return order_collected;
        }
        else{
           removeOrder(order_collected);
           return get_order(price);

        }   
    }
    return std::nullopt; 
    
}

int Orderbook::get_size() const {
    return orderbook.size();
}

std::deque<Order>* Orderbook::get_orders_at_price(float price){
    auto it = orderbook.find(price);
    if (it != orderbook.end()) {
        return &it->second; // Return the deque of orders for the given price
    }
    return nullptr; // Return a null pointer if no orders are found at this price
}