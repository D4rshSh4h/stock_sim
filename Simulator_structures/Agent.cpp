#include "Agent.h"
#include "DecisionEngine.h"
#include "Simulator.h"
#include <optional>

Agent::Agent(Simulator &sim, const IAgentDecisionEngine &decision_engine, int id,
             float cash, int shares, AgentState state)
    : simulator(sim), decision_engine(decision_engine), id(id), cash(cash),
      shares(shares), state(state) {}
Agent::~Agent() {}

void Agent::place_order(Order &order) { 
  if (order.getTradeType() == OrderType::Buy) {
    simulator.simulator_buy_trade(order);
    cash -= int_to_float_price(order.getPrice())*order.getQty(); // Deduct cash immediately when placing a buy order
  } else {
    simulator.simulator_sell_trade(order);
    shares -= order.getQty(); 
  }
}


void Agent::run() {
  if (cash == 0 && shares == 0) {
    state = AgentState::Liquid;
    return;
  }

  AgentDecisionContext ctx{
      id,
      cash,
      shares,
      simulator.get_current_price(),
      simulator.get_current_time(),
  };
  std::optional<Order> order_opt = decision_engine.decide_order(ctx);
  if (order_opt.has_value()) {
    place_order(*order_opt);
    if (order_opt->getTradeType() == OrderType::Buy) {
      state = AgentState::Buying;
    } else {
      state = AgentState::Selling;
    }
  }
}



int Agent::get_id() const { return id; }

AgentState Agent::get_state() const { return state; }
float Agent::get_cash() const { return cash; }
int Agent::get_shares() const { return shares; }

void Agent::run_agent() {
  run();
}

void Agent::change_state(AgentState new_state, float added_cash, int added_shares) {
  //apply_state_change(new_state, added_cash, added_shares);
  state = new_state;
  cash += added_cash;
  shares += added_shares;
}