#ifndef TRADES_H
#define TRADES_H

#include "../Order_structures/orderbook.h"
#include "../Simulator_structures/TradeObserver.h"

void set_trade_observer(TradeObserver* observer);
bool buy_trade(Order &order, Orderbook &buy, Orderbook &sell, TradeObserver& observer);
bool sell_trade(Order &order, Orderbook &buy, Orderbook &sell, TradeObserver& observer);
void sweep_book(Orderbook& buy, Orderbook& sell, TradeObserver& observer);
bool match_orders(Order &order, Orderbook &buy, Orderbook &sell);
void log_trade(Order &buyer, Order &seller, float price, float spread, int qty);
void prepare_file(void);

#endif // TRADES_H