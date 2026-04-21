#include "Simulator_structures/Agent.h"
#include "Simulator_structures/Simulator.h"
#include "Trade_logic/trades.h"
#include "Trade_logic/TradeLogger.h"
#include "config.h"
#include "file_handler.h"
#include <iostream>

// TODO need to create a mechanism to add cash mid simulation

int main() {
  // Setup
  Simulator simulator(INITIAL_PRICE, TOTAL_CASH, TOTAL_SHARES);
  prepare_file();
  simulator.initialize_agents(NO_AGENTS);
  std::cout << "Finished prep starting sim" << std::endl;

  Orderbook &buy_book_ref = simulator.get_buy_book();
  Orderbook &sell_book_ref = simulator.get_sell_book();

  // Simulation
  // TODO track bid-ask prices
  for (int i = 0; i < TICKS; i++) {
    simulator.update_time();
    std::vector<int> shuffled_ids = simulator.shuffle_agent_ids();
    for (int id : shuffled_ids) {
      Agent *agent = simulator.get_agent(id);
      if (agent) {
        agent->run();
      }
    }

    sweep_book(buy_book_ref, sell_book_ref, simulator);
    simulator.expire_timed_out_orders(TIMEOUT_DURATION);

    // simulator.log_price();
    simulator.log_volume();
    simulator.reset_volume();
  }
  TradeLogger::instance().flush();
  print_volume(simulator.get_volume_time_log());
}
