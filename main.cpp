#include "Simulator_structures/Agent.h"
#include "Simulator_structures/Simulator.h"
#include "Simulator_structures/World.h"
#include "Simulator_structures/Company.h"
#include "Trade_logic/trades.h"
#include "Trade_logic/TradeLogger.h"
#include "config.h"
#include "file_handler.h"
#include <cmath>
#include <iostream>
#include <optional>
#include <vector>

//TODO need to create a mechanism to add cash mid simulation
//TODO cross asset simulations
//TODO need to create an interface (e.g. app) where my project can be tried out
//TODO let agents set thier own timeout durations

/*
Tick Structure:
STATIC TICK STRUCTURE FOR NOW
5 ticks = 1 block
Company releases earning every 5 blocks
Interest rates change every 3 blocks
A full business cycle every 4 blocks
*/

namespace {
float book_price_or_missing(const std::optional<int>& px) {
  if (!px.has_value()) return NO_BOOK_PRICE_FLOAT;
  return int_to_float_price(*px);
}
} // namespace

int main() {
  // Setup
  Simulator simulator(INITIAL_PRICE, TOTAL_CASH, TOTAL_SHARES);
  World world;
  Company company(world);

  prepare_file();
  set_trade_observer(&simulator);
  simulator.initialize_agents(NO_AGENTS);
  log_agent_snapshots(simulator, 0, simulator.get_current_price());
  std::cout << "Finished prep starting sim" << std::endl;

  Orderbook &buy_book_ref = simulator.get_buy_book();
  Orderbook &sell_book_ref = simulator.get_sell_book();

  int current_time;
  // Simulation
  for (int i = 0; i < TICKS; i++) {
    world.update_time();
    current_time = world.get_time();
    auto p = company.send_data();
    std::vector<int> shuffled_ids = simulator.shuffle_agent_ids();
    for (int id : shuffled_ids) {
      Agent *agent = simulator.get_agent(id);
      if (agent) {
        agent->run(current_time, p);
      }
    }

    sweep_book(buy_book_ref, sell_book_ref, simulator);
    simulator.find_order_timeouts(current_time, TIMEOUT_DURATION);

    simulator.log_price(current_time);
    simulator.log_volume(current_time);

    const float best_bid = book_price_or_missing(buy_book_ref.best_bid_price());
    const float best_ask = book_price_or_missing(sell_book_ref.best_ask_price());
    log_tick(current_time,
             simulator.get_current_price(),
             simulator.get_tick_volume(),
             best_bid,
             best_ask,
             buy_book_ref.get_size(),
             sell_book_ref.get_size(),
             buy_book_ref.total_qty(),
             sell_book_ref.total_qty(),
             p.first,
             p.second,
             std::sin(world.get_business_cycle()));

    // Snapshot agents each tick for wealth / allocation paths
    log_agent_snapshots(simulator, current_time, simulator.get_current_price());

    simulator.reset_volume();
  }
  TradeLogger::instance().flush();
  print_volume(simulator.get_volume_time_log());
  print_cycle(world.get_business_cycle_map(), world.get_interest_rate_map());
  std::cout << "Simulation complete. CSV logs written." << std::endl;
}
