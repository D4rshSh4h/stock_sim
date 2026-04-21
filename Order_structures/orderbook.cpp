#include "orderbook.h"
#include "../config.h"
#include <iostream>

Orderbook::Orderbook() {}
Orderbook::~Orderbook() {}

std::shared_ptr<Order> Orderbook::addOrder(const Order &order) {
    auto& orders_at_price = orderbook[order.getPrice()];
    auto order_ptr = std::make_shared<Order>(order);
    orders_at_price.push_back(order_ptr);
    return order_ptr;
}
void Orderbook::removePrice(const Order &order) {
    int price_remove = order.getPrice();
    auto check = orderbook.erase(price_remove);
    if (check == 0) {
        std::cout << "Order not found in orderbook." << std::endl;
    }

}

//Removes the order at the front of the queue for the given price. If the queue is empty, removes the price from the orderbook
void Orderbook::removeOrder(const Order &order) {
    int price_index = order.getPrice();
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

int Orderbook::find_lowest_price() const {
    if (orderbook.empty()) {
        return NO_BOOK_PRICE_INT;
    }
    return orderbook.begin()->first; // The lowest price is the first key in the map
}   

int Orderbook::find_highest_price() const {
    if (orderbook.empty()) {
        return NO_BOOK_PRICE_INT;
    }
    return orderbook.rbegin()->first; // The highest price is the last key in the map
}

std::optional<int> Orderbook::best_ask_price() const {
    int ask = find_lowest_price();
    if (!is_valid_book_price(ask)) {
        return std::nullopt;
    }
    return ask;
}

std::optional<int> Orderbook::best_bid_price() const {
    int bid = find_highest_price();
    if (!is_valid_book_price(bid)) {
        return std::nullopt;
    }
    return bid;
}

std::optional<Order> Orderbook::get_order(int price) {
    auto it = orderbook.find(price);
    if(it == orderbook.end() || it->second.empty()){
        return std::nullopt; // Return an empty optional if no orders are found at this price
    }
    else{
        while (!it->second.empty()) {
            std::shared_ptr<Order> order_collected = it->second.front();
            if(order_collected->getStatus() == OrderStatus::Active){
                return *order_collected;
            }
            removeOrder(*order_collected);
        }   
    }
    return std::nullopt; 
    
}

int Orderbook::get_size() const {
    return orderbook.size();
}

std::list<std::shared_ptr<Order>>* Orderbook::get_orders_at_price(int price){
    auto it = orderbook.find(price);
    if (it != orderbook.end()) {
        return &it->second; // Return the list of orders for the given price
    }
    return nullptr; // Return a null pointer if no orders are found at this price
}