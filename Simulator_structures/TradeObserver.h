#ifndef TRADEOBSERVER_H
#define TRADEOBSERVER_H
#include "../Order_structures/order.h"
#include <memory>

//Virtual class inherited by simulator. Exists so that trades.cpp can inherit these and access the methods
class TradeObserver{
    public:
        virtual void on_trade_agent_state(const Order& buy_order, const Order& sell_order, float price, float spread, int qty) = 0;
        virtual void update_time_order_index(std::shared_ptr<Order> order_ptr) = 0;
        virtual ~TradeObserver() = default;
};

#endif // TRADEOBSERVER_H  