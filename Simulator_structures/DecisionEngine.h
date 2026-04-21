#ifndef DECISION_ENGINE_H
#define DECISION_ENGINE_H

#include "../Order_structures/order.h"
#include "../config.h"
#include <optional>

struct AgentDecisionContext {
  int agent_id;
  float cash;
  int shares;
  float current_price;
  int current_time;
};

class IAgentDecisionEngine {
public:
  virtual ~IAgentDecisionEngine() = default;
  virtual std::optional<Order> decide_order(const AgentDecisionContext &ctx) const = 0;
};

class RandomDecisionEngine final : public IAgentDecisionEngine {
public:
  std::optional<Order> decide_order(const AgentDecisionContext &ctx) const override;

private:
  bool decide2() const;
  int decide3() const;
  std::optional<Order> agent_buy(const AgentDecisionContext &ctx) const;
  std::optional<Order> agent_sell(const AgentDecisionContext &ctx) const;
};

#endif // DECISION_ENGINE_H
