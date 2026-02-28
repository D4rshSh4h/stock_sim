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
    void removeOrder(Order order);

private:
    std::map<int, std::queue<Order>> orderbook;
};


#endif //ORDERBOOK_H
