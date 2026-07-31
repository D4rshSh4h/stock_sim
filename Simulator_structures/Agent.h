#ifndef AGENT_H
#define AGENT_H

#include "../Order_structures/order.h"
#include "../config.h"
#include <optional>
#include <utility>

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
  AgentBehaviour behaviour;
  int growth_rate;
  int time_horizon; //This is BLOCKS

public:
  Agent(Simulator &sim, const IAgentDecisionEngine &decision_engine, int id,
        float cash, int shares, AgentState state, AgentBehaviour behaviour, int time_horizon);
  ~Agent();
  void run(int time, std::pair<int, float>& world_data);
  void apply_state_change(AgentState new_state, float added_cash, int added_shares);
  int get_id() const;
  AgentState get_state() const;
  float get_cash() const;
  int get_shares() const;
  void set_growth_rate(int rate);
  void set_time_horizon(int horizon);
  int get_growth_rate() const;
  int get_time_horizon() const;
  AgentBehaviour get_behaviour() const;

  // Compatibility wrappers for existing call sites.
  void run_agent();
  void change_state(AgentState new_state, float added_cash, int added_shares);
};

#endif // AGENT_H