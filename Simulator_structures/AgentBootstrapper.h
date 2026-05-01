#ifndef AGENT_BOOTSTRAPPER_H
#define AGENT_BOOTSTRAPPER_H

#include "DecisionEngine.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

class Simulator;
class Agent;

class AgentBootstrapper {
public:
  void initialize_agents(
      Simulator &simulator,
      std::unordered_map<int, std::unique_ptr<Agent>> &agents,
      std::vector<int> &agent_ids, int no_agents, float total_cash,
      int total_shares, float current_price,
      const std::function<const IAgentDecisionEngine &(int)> &select_engine) const;

private:
  size_t choose_split_index(int no_agents) const;
  bool allocate_cash_agents(std::vector<std::unique_ptr<Agent>> &temp_agent_vector,
                            int no_cash_agents, float total_cash,
                            float current_price) const;
  void allocate_share_agents(std::vector<std::unique_ptr<Agent>> &temp_agent_vector,
                             int no_agents, size_t split_index,
                             int total_shares) const;
  void register_agents(
      std::vector<std::unique_ptr<Agent>> &temp_agent_vector,
      std::unordered_map<int, std::unique_ptr<Agent>> &agents,
      std::vector<int> &agent_ids) const;
};

#endif // AGENT_BOOTSTRAPPER_H
