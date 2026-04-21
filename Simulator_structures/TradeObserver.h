#ifndef TRADEOBSERVER_H
#define TRADEOBSERVER_H
#include "../Order_structures/order.h"
#include <memory>

class TradeObserver{
    public:
        virtual void on_trade_executed(const Order& buy_order, const Order& sell_order, float price, float spread) = 0;
        virtual void track_resting_order_for_timeout(std::shared_ptr<Order> order_ptr) = 0;

        // Compatibility wrappers for existing call sites.
        virtual void on_trade_agent_state(const Order& buy_order, const Order& sell_order, float price, float spread) {
            on_trade_executed(buy_order, sell_order, price, spread);
        }

        virtual void update_time_order_index(std::shared_ptr<Order> order_ptr) {
            track_resting_order_for_timeout(order_ptr);
        }
        virtual ~TradeObserver() = default;
};

#endif // TRADEOBSERVER_H
