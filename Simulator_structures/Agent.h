#ifndef AGENT_H
#define AGENT_H

#include "../Order_structures/order.h"
#include "../config.h"
#include <optional>

class Simulator;
class IAgentDecisionEngine;

class Agent {
private:
  Simulator &simulator;
  const IAgentDecisionEngine &decision_engine;
  int id;
  float cash;
  int shares;
  AgentState state;
  void place_order(Order &order);

public:
  Agent(Simulator &sim, const IAgentDecisionEngine &decision_engine, int id,
        float cash, int shares, AgentState state);
  ~Agent();
  void run(int time);
  void apply_state_change(AgentState new_state, float added_cash, int added_shares);
  int get_id() const;
  AgentState get_state() const;
  float get_cash() const;
  int get_shares() const;

  // Compatibility wrappers for existing call sites.
  void run_agent();
  void change_state(AgentState new_state, float added_cash, int added_shares);
};

#endif // AGENT_H