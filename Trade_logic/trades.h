#ifndef TRADES_H
#define TRADES_H

#include "../Order_structures/orderbook.h"
#include "../Simulator_structures/TradeObserver.h"

class MatchingEngine {
public:
    explicit MatchingEngine(TradeObserver* observer);
    bool execute_buy_order(Order &order, Orderbook &buy, Orderbook &sell);
    bool execute_sell_order(Order &order, Orderbook &buy, Orderbook &sell);
    void sweep_orderbooks(Orderbook& buy, Orderbook& sell);

    // Compatibility names.
    bool buy_trade(Order &order, Orderbook &buy, Orderbook &sell);
    bool sell_trade(Order &order, Orderbook &buy, Orderbook &sell);
    void sweep_book(Orderbook& buy, Orderbook& sell);
    float calculate_spread(Order &buy_order, Order &sell_order);

private:
    TradeObserver* observer_;
    bool match_orders(Order &order, Orderbook &buy, Orderbook &sell);
    void log_trade(Order &buyer, Order &seller, float price, float spread);
};

bool buy_trade(Order &order, Orderbook &buy, Orderbook &sell, TradeObserver& observer);
bool sell_trade(Order &order, Orderbook &buy, Orderbook &sell, TradeObserver& observer);
void sweep_book(Orderbook& buy, Orderbook& sell, TradeObserver& observer);

void prepare_file(void);

#endif // TRADES_H