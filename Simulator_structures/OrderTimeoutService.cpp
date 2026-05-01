#include "OrderTimeoutService.h"
#include "Agent.h"
#include "../config.h"

void OrderTimeoutService::track_resting_order(std::shared_ptr<Order> order_ptr) {
  if (order_ptr) {
    timeout_queue_.push(order_ptr);
  }
}

void OrderTimeoutService::expire_timed_out_orders(
    int current_time, int timeout_duration,
    const std::function<Agent *(int)> &find_agent) {
  while (!timeout_queue_.empty()) {
    std::shared_ptr<Order> order = timeout_queue_.front().lock();
    if (!order) {
      timeout_queue_.pop();
      continue;
    }
    if (order->getStatus() != OrderStatus::Active) {
      timeout_queue_.pop();
      continue;
    }
    if (current_time - order->getTimePlaced() < timeout_duration) {
      break;
    }

    order->timeout_order();
    order->cancel_order();
    Agent *agent_change = find_agent(order->getId());
    if (agent_change) {
      if (order->getTradeType() == OrderType::Buy) {
        agent_change->change_state(agent_change->get_state(),
                                   int_to_float_price(order->getPrice()), 0);
      } else {
        agent_change->change_state(agent_change->get_state(), 0, 1);
      }
    }
    timeout_queue_.pop();
  }
}
