#ifndef TRADE_SETTLEMENT_SERVICE_H
#define TRADE_SETTLEMENT_SERVICE_H

#include "../Order_structures/order.h"
#include <functional>

class Agent;

class TradeSettlementService {
public:
  void settle_trade(const Order &buy_order, const Order &sell_order, float price,
                    float spread, float &current_price, int &volume,
                    const std::function<Agent *(int)> &find_agent) const;
};

#endif // TRADE_SETTLEMENT_SERVICE_H
