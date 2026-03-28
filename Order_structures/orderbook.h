#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "order.h"
#include <map>
#include <optional>
#include <queue>

class Orderbook {
public:
  Orderbook();
  ~Orderbook();
  
  int get_size() const;
  void addOrder(const Order &order);
  void removePrice(const Order &order);
  void removeOrder(const Order &order);
  bool is_empty() const;
  float find_lowest_price() const;
  float find_highest_price() const;
  std::optional<Order> get_order(float price) const;

private:
  std::map<float, std::queue<Order>> orderbook;
};

#endif // ORDERBOOK_H
