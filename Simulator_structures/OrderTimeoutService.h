#ifndef ORDER_TIMEOUT_SERVICE_H
#define ORDER_TIMEOUT_SERVICE_H

#include "../Order_structures/order.h"
#include <functional>
#include <memory>
#include <queue>

class Agent;

class OrderTimeoutService {
public:
  void track_resting_order(std::shared_ptr<Order> order_ptr);
  void expire_timed_out_orders(int current_time, int timeout_duration,
                               const std::function<Agent *(int)> &find_agent);

private:
  std::queue<std::weak_ptr<Order>> timeout_queue_;
};

#endif // ORDER_TIMEOUT_SERVICE_H
