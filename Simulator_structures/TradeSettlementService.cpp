#include "TradeSettlementService.h"
#include "Agent.h"
#include "../config.h"

void TradeSettlementService::settle_trade(
    const Order &buy_order, const Order &sell_order, float price, float spread,
    float &current_price, int &volume,
    const std::function<Agent *(int)> &find_agent) const {
  if (price == 0.00f) {
    current_price = price + BUFFER;
  } else {
    current_price = price;
  }

  Agent *buyer = find_agent(buy_order.getId());
  Agent *seller = find_agent(sell_order.getId());

  if (buyer) {
    buyer->change_state(AgentState::Holding, spread, 1);
  }
  if (seller) {
    seller->change_state(AgentState::Liquid, price, 0);
  }
  volume++;
}
