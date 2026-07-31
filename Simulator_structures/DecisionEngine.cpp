#include "DecisionEngine.h"
#include "rng.h"
#include <random>
#include <cmath>
#include <algorithm>

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

float IVDecisionEngine::calculate_intrinsic_value(const AgentDecisionContext &ctx) const {
  float intr_value = 0;
  // g_s is stored in thousandths (e.g. 200 -> 20% short-term growth)
  float short_rate = ctx.g_s / 1000.0f;
  float temp_calc;
  for(int i = 1; i <= ctx.n; i++){
    temp_calc = ((1+short_rate)/(1+ctx.interest_rate));
    intr_value += std::pow(temp_calc, i);
  }
  float TV = calculate_terminal_value(ctx);
  return (intr_value*ctx.company_fcf) + TV;
 
}

float IVDecisionEngine::calculate_terminal_value(const AgentDecisionContext &ctx) const {
  // Require a minimum equity risk premium over GDP growth to avoid explosive TV.
  float excess_return = ctx.interest_rate - GDP_GROWTH_RATE;
  constexpr float min_excess = 0.005f;
  if (excess_return < min_excess) {
    excess_return = min_excess;
  }
  float s_r = ctx.g_s / 1000.0f;
  float temp1 = (1 + GDP_GROWTH_RATE) / excess_return;
  float temp2 = (1 + s_r) / (1 + ctx.interest_rate);
  float temp3 = std::pow(temp2, ctx.n);
  return ctx.company_fcf * temp1 * temp3;

}

// Gap = IV per share - market price
double IVDecisionEngine::computeGap(double iv, float mkt_price) const {
    return (iv/TOTAL_SHARES) - mkt_price;
}

// Returns {price, qty}. qty: positive=buy, negative=sell, 0=hold (price=mkt_price on hold).
// Threshold is the minimum gap to trigger a trade.
// delta is a fraction of mkt_price (same convention as RandomDecisionEngine / RANGE).
std::pair<float, int> IVDecisionEngine::getPriceAndQty(double gap, float mkt_price, float threshold, double iv_per_share,
             float cash, int shares_held, int max_qty, float delta, double skew_power) const {
    if (gap <= threshold && gap >= -threshold) return {mkt_price, 0};

    bool is_buy = gap > threshold;

    double normalised_gap = std::min(std::abs(gap) / std::max(static_cast<double>(mkt_price), 1e-6), 1.0);

    static thread_local std::uniform_real_distribution<double> unif(0.0, 1.0);
    double u = unif(get_rng());
    double lower, upper, t;
    double price_delta = static_cast<double>(delta) * mkt_price;
    if (is_buy) {
        lower = mkt_price - price_delta;
        upper = iv_per_share;
        t = std::pow(u, skew_power);
    } else {
        lower = iv_per_share;
        upper = mkt_price + price_delta;
        t = std::pow(u, 1.0 / skew_power);
    }
    if (upper < lower) std::swap(lower, upper);
    if (lower < 0.0) lower = 0.0;
    float price = static_cast<float>(lower + (upper - lower) * t);
    if (price <= 0.0f) return {mkt_price, 0};

    // Capacity must be resource-correct: cash for buys, shares for sells.
    // (Previously always used cash/price, so cash=0 share-holders could never sell.)
    int capacity = is_buy
        ? static_cast<int>(std::floor(cash / price))
        : shares_held;
    if (max_qty > 0) {
        capacity = std::min(capacity, max_qty);
    }

    double raw_qty = normalised_gap * static_cast<double>(capacity);
    double resource_cap = is_buy
        ? cash / std::max(iv_per_share, 1e-6)
        : static_cast<double>(shares_held);
    double clamped = std::min({raw_qty, resource_cap, static_cast<double>(capacity)});
    int qty = static_cast<int>(std::floor(std::max(clamped, 0.0)));

    return {price, is_buy ? qty : -qty};
}

//Need to check order status - decide what to do if pending
std::optional<Order> IVDecisionEngine::decide_order(const AgentDecisionContext &ctx) const {
  float IV = calculate_intrinsic_value(ctx);
  double gap = computeGap(IV, ctx.current_price);
  auto pair = getPriceAndQty(gap, ctx.current_price, THRESHOLD, IV/TOTAL_SHARES, ctx.cash, ctx.shares, 0, RANGE);

  if (pair.second != 0) {
    return std::make_optional(Order(float_to_int_price(pair.first), ctx.agent_id,
                                  pair.second > 0 ? OrderType::Buy : OrderType::Sell,
                                  ctx.current_time, OrderStatus::Active, std::abs(pair.second)));
  }
  return std::nullopt;
}