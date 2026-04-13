#ifndef TRADEOBSERVER_H
#define TRADEOBSERVER_H
#include "../Order_structures/order.h"

class TradeObserver{
    public:
        virtual void on_trade_agent_state(const Order& buy_order, const Order& sell_order, float price, float spread) = 0;
        virtual ~TradeObserver() = default;
};

#endif // TRADEOBSERVER_H
