#ifndef DECISION_ENGINE_H
#define DECISION_ENGINE_H

#include "../Order_structures/order.h"
#include "../config.h"
#include <optional>
#include <utility>


struct AgentDecisionContext {
  int agent_id;
  float cash;
  int shares;
  float current_price;
  int current_time;
  int company_fcf;
  float interest_rate;
  int g_s;
  int n;
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

class IVDecisionEngine final : public IAgentDecisionEngine {
  public:
    std::optional<Order> decide_order(const AgentDecisionContext &ctx) const override;
  
  private:
    float calculate_intrinsic_value(const AgentDecisionContext &ctx) const;
    float calculate_terminal_value(const AgentDecisionContext &ctx) const;
    double computeGap(double iv, float mkt_price) const;
    std::pair<float, int> IVDecisionEngine::getPriceAndQty(double gap, float mkt_price, float threshold, double iv_per_share,
             float cash, int shares_held, int max_qty, float delta, std::mt19937& rng, double skew_power = 2.0) const;

};

#endif // DECISION_ENGINE_H
