#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <map>
#include <queue>
#include "order.h"

class Orderbook {
public:
    Orderbook();
    ~Orderbook();

    void addOrder(Order order);
    void removePrice(Order order);
    void removeOrder(Order order);
    bool is_empty() const {};
    int find_lowest_price() const {};
    int find_highest_price() const {};

private:
    std::map<int, std::queue<Order>> orderbook;
};


#endif //ORDERBOOK_H
