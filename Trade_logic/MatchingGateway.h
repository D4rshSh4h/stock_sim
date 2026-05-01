#ifndef MATCHING_GATEWAY_H
#define MATCHING_GATEWAY_H

#include "../Order_structures/orderbook.h"

class Order;
class TradeObserver;

class MatchingGateway {
public:
  bool execute_buy(Order &order, Orderbook &buy_book, Orderbook &sell_book,
                   TradeObserver &observer) const;
  bool execute_sell(Order &order, Orderbook &buy_book, Orderbook &sell_book,
                    TradeObserver &observer) const;
  void sweep(Orderbook &buy_book, Orderbook &sell_book,
             TradeObserver &observer) const;
};

#endif // MATCHING_GATEWAY_H
