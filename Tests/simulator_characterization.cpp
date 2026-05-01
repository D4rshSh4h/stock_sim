#include "../Simulator_structures/Simulator.h"
#include "../Simulator_structures/Agent.h"
#include "../config.h"
#include <cassert>
#include <iostream>
#include <memory>

namespace {

void test_trade_settlement_updates_market_and_agents() {
    Simulator simulator(INITIAL_PRICE, TOTAL_CASH, TOTAL_SHARES);

    Order buy_order(float_to_int_price(100.0f), 0, OrderType::Buy, 1, OrderStatus::Active);
    Order sell_order(float_to_int_price(99.0f), 1, OrderType::Sell, 1, OrderStatus::Active);
    simulator.on_trade_executed(buy_order, sell_order, 100.0f, 1.0f);

    assert(simulator.get_current_price() == 100.0f);
    simulator.log_volume();
    assert(simulator.get_volume(0) == 1);
}

void test_timeout_refund_for_buy_order() {
    Simulator simulator(INITIAL_PRICE, TOTAL_CASH, TOTAL_SHARES);
    auto order = std::make_shared<Order>(float_to_int_price(25.0f), 0, OrderType::Buy, 0, OrderStatus::Active);
    simulator.track_resting_order_for_timeout(order);

    for (int i = 0; i < 6; ++i) {
        simulator.update_time();
    }
    simulator.expire_timed_out_orders(5);

    assert(order->getStatus() == OrderStatus::Cancelled);
}

} // namespace

int main() {
    test_trade_settlement_updates_market_and_agents();
    test_timeout_refund_for_buy_order();
    std::cout << "Simulator characterization tests passed.\n";
    return 0;
}
