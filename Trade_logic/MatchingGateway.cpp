#include "MatchingGateway.h"
#include "trades.h"

bool MatchingGateway::execute_buy(Order &order, Orderbook &buy_book,
                                  Orderbook &sell_book,
                                  TradeObserver &observer) const {
  return buy_trade(order, buy_book, sell_book, observer);
}

bool MatchingGateway::execute_sell(Order &order, Orderbook &buy_book,
                                   Orderbook &sell_book,
                                   TradeObserver &observer) const {
  return sell_trade(order, buy_book, sell_book, observer);
}

void MatchingGateway::sweep(Orderbook &buy_book, Orderbook &sell_book,
                            TradeObserver &observer) const {
  sweep_book(buy_book, sell_book, observer);
}
