#include "AgentBootstrapper.h"
#include "Agent.h"
#include "Simulator.h"
#include "rng.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>

namespace {
auto &gen = get_rng();
}

void AgentBootstrapper::initialize_agents(
    Simulator &simulator, std::unordered_map<int, std::unique_ptr<Agent>> &agents,
    std::vector<int> &agent_ids, int no_agents, float total_cash, int total_shares,
    float current_price,
    const std::function<const IAgentDecisionEngine &(int)> &select_engine) const {
  std::vector<std::unique_ptr<Agent>> temp_agent_vector;
  for (int i = 0; i < no_agents; i++) {
    const IAgentDecisionEngine &engine = select_engine(i);
    temp_agent_vector.emplace_back(std::make_unique<Agent>(
        simulator, engine, i, 0.00f, 0, AgentState::Uninitialized));
  }

  std::shuffle(temp_agent_vector.begin(), temp_agent_vector.end(), gen);
  size_t split_index = choose_split_index(no_agents);
  int no_cash_agents = static_cast<int>(split_index);
  if (!allocate_cash_agents(temp_agent_vector, no_cash_agents, total_cash,
                            current_price)) {
    return;
  }
  allocate_share_agents(temp_agent_vector, no_agents, split_index, total_shares);
  register_agents(temp_agent_vector, agents, agent_ids);
}

size_t AgentBootstrapper::choose_split_index(int no_agents) const {
  std::uniform_int_distribution<size_t> dist(1, no_agents - 1);
  return dist(gen);
}

bool AgentBootstrapper::allocate_cash_agents(
    std::vector<std::unique_ptr<Agent>> &temp_agent_vector, int no_cash_agents,
    float total_cash, float current_price) const {
  float cuts_cash = total_cash - (no_cash_agents * current_price);
  if (cuts_cash < 0) {
    std::cout << "Not enough total cash to give each cash agent enough to buy 1 "
                 "share at initial price. Please adjust total cash or number of "
                 "agents."
              << std::endl;
    return false;
  }
  std::vector<float> cut_points_cash;
  cut_points_cash.reserve(no_cash_agents - 1);
  std::uniform_real_distribution<float> cut_dist(0, cuts_cash);
  for (int i = 0; i < no_cash_agents - 1; i++) {
    cut_points_cash.push_back(cut_dist(gen));
  }
  std::sort(cut_points_cash.begin(), cut_points_cash.end());
  for (int i = 0; i < no_cash_agents; i++) {
    if (i == 0) {
      temp_agent_vector[i]->change_state(AgentState::Buying,
                                         cut_points_cash[i] + current_price, 0);
    } else if (i == no_cash_agents - 1) {
      temp_agent_vector[i]->change_state(
          AgentState::Buying, cuts_cash - cut_points_cash[i - 1] + current_price,
          0);
    } else {
      temp_agent_vector[i]->change_state(
          AgentState::Buying, cut_points_cash[i] - cut_points_cash[i - 1] + current_price, 0);
    }
  }
  return true;
}

void AgentBootstrapper::allocate_share_agents(
    std::vector<std::unique_ptr<Agent>> &temp_agent_vector, int no_agents,
    size_t split_index, int total_shares) const {
  int no_share_agents = no_agents - static_cast<int>(split_index);
  int cuts_shares = total_shares - no_share_agents;
  std::vector<int> cut_points_shares;
  cut_points_shares.reserve(no_share_agents - 1);
  std::vector<int> positions(cuts_shares);
  std::iota(positions.begin(), positions.end(), 1);
  std::sample(positions.begin(), positions.end(),
              std::back_inserter(cut_points_shares), no_share_agents - 1, gen);
  std::sort(cut_points_shares.begin(), cut_points_shares.end());
  for (int i = static_cast<int>(split_index); i < no_agents; i++) {
    if (static_cast<size_t>(i) == split_index) {
      temp_agent_vector[i]->change_state(
          AgentState::Selling, 0,
          cut_points_shares[i - static_cast<int>(split_index)] + 1);
    } else if (i == no_agents - 1) {
      temp_agent_vector[i]->change_state(
          AgentState::Selling, 0,
          cuts_shares - cut_points_shares[i - static_cast<int>(split_index) - 1] +
              1);
    } else {
      temp_agent_vector[i]->change_state(
          AgentState::Selling, 0,
          cut_points_shares[i - static_cast<int>(split_index)] -
              cut_points_shares[i - static_cast<int>(split_index) - 1] + 1);
    }
  }
}

void AgentBootstrapper::register_agents(
    std::vector<std::unique_ptr<Agent>> &temp_agent_vector,
    std::unordered_map<int, std::unique_ptr<Agent>> &agents,
    std::vector<int> &agent_ids) const {
  for (auto &agent_ptr : temp_agent_vector) {
    int id = agent_ptr->get_id();
    agents[id] = std::move(agent_ptr);
  }
  agent_ids.clear();
  for (const auto &pair : agents) {
    agent_ids.push_back(pair.first);
  }
}
