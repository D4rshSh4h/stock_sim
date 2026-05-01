#include "SimulationRunner.h"
#include "Agent.h"
#include "Simulator.h"
#include "../Trade_logic/MatchingGateway.h"
#include <vector>

void SimulationRunner::run(Simulator &simulator, int ticks,
                           int timeout_duration) const {
  const MatchingGateway gateway;
  Orderbook &buy_book_ref = simulator.get_buy_book();
  Orderbook &sell_book_ref = simulator.get_sell_book();

  for (int i = 0; i < ticks; i++) {
    simulator.update_time();
    std::vector<int> shuffled_ids = simulator.shuffle_agent_ids();
    for (int id : shuffled_ids) {
      Agent *agent = simulator.get_agent(id);
      if (agent) {
        agent->run();
      }
    }

    gateway.sweep(buy_book_ref, sell_book_ref, simulator);
    simulator.expire_timed_out_orders(timeout_duration);
    simulator.log_volume();
    simulator.reset_volume();
  }
}
