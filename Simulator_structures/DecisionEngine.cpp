#include "DecisionEngine.h"
#include "rng.h"
#include <random>
#include <cmath>

bool RandomDecisionEngine::decide2() const {
  std::bernoulli_distribution dist(0.5);
  return dist(get_rng());
}

int RandomDecisionEngine::decide3() const {
  std::uniform_int_distribution<> dist(0, 2);
  return dist(get_rng());
}

//Creates a buy order from a distribution of mkt price +- delta
std::optional<Order> RandomDecisionEngine::agent_buy(const AgentDecisionContext &ctx) const {
  std::uniform_real_distribution<float> price_dist;
  float delta = RANGE * ctx.current_price;
  if (ctx.current_price == 0.00f) {
    price_dist = std::uniform_real_distribution<float>(0, RANGE);
  } else {
    price_dist = std::uniform_real_distribution<float>(ctx.current_price - delta,
                                                       ctx.current_price + delta);
  }

  float price = price_dist(get_rng());
  if (ctx.cash < price) {
    return std::nullopt;
  }
  int max_shares = std::floor(ctx.cash/price);
  int shares_to_buy = std::uniform_int_distribution<>(1, max_shares)(get_rng());
  return std::make_optional(Order(float_to_int_price(price), ctx.agent_id,
                                  OrderType::Buy, ctx.current_time,
                                  OrderStatus::Active, shares_to_buy));
}

//Creates a sell order from a distribution of mkt price +- delta
std::optional<Order> RandomDecisionEngine::agent_sell(const AgentDecisionContext &ctx) const {
  std::uniform_real_distribution<float> price_dist;
  float delta = RANGE * ctx.current_price;
  if (ctx.current_price == 0.00f) {
    price_dist = std::uniform_real_distribution<float>(0, RANGE);
  } else {
    price_dist = std::uniform_real_distribution<float>(ctx.current_price - delta,
                                                       ctx.current_price + delta);
  }

  float price = price_dist(get_rng());
  if (ctx.shares < 1) {
    return std::nullopt;
  }
  int shares_to_sell = std::uniform_int_distribution<>(1, ctx.shares)(get_rng());
  return std::make_optional(Order(float_to_int_price(price), ctx.agent_id,
                                  OrderType::Sell, ctx.current_time,
                                  OrderStatus::Active, shares_to_sell));
}

//Decides whether the agent buys, sells or holds. Takes agents resources into account
std::optional<Order> RandomDecisionEngine::decide_order(const AgentDecisionContext &ctx) const {
  if (ctx.shares == 0) {
    bool decision = decide2();
    if (decision) {
      return agent_buy(ctx);
    }
    return std::nullopt;
  }

  if (ctx.cash == 0) {
    bool decision = decide2();
    if (decision) {
      return agent_sell(ctx);
    }
    return std::nullopt;
  }

  int decision = decide3();
  if (decision == 0) {
    return agent_buy(ctx);
  }
  if (decision == 1) {
    return agent_sell(ctx);
  }
  return std::nullopt;
}
