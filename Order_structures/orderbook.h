#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "order.h"
#include <map>
#include <optional>
#include <list>
#include <memory>

class Orderbook {
public:
  Orderbook();
  ~Orderbook();
  
  int get_size() const;
  std::shared_ptr<Order> addOrder(const Order &order);
  void removePrice(const Order &order);
  void removeOrder(const Order &order);
  bool is_empty() const;
  int find_lowest_price() const;
  int find_highest_price() const;
  std::optional<int> best_ask_price() const;
  std::optional<int> best_bid_price() const;
  std::optional<Order> get_order(int price);
  std::list<std::shared_ptr<Order>>* get_orders_at_price(int price);

private:
  std::map<int, std::list<std::shared_ptr<Order>>> orderbook;
};

#endif // ORDERBOOK_H
