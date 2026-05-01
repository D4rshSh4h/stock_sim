#include "Simulator_structures/Agent.h"
#include "Simulator_structures/Simulator.h"
#include "Simulator_structures/SimulationOutputSink.h"
#include "Simulator_structures/SimulationRunner.h"
#include "config.h"
#include <iostream>

// TODO need to create a mechanism to add cash mid simulation

int main() {
  // Setup
  Simulator simulator(INITIAL_PRICE, TOTAL_CASH, TOTAL_SHARES);
  SimulationOutputSink output_sink;
  output_sink.prepare();
  simulator.initialize_agents(NO_AGENTS);
  std::cout << "Finished prep starting sim" << std::endl;

  SimulationRunner runner;
  runner.run(simulator, TICKS, TIMEOUT_DURATION);
  output_sink.finalize(simulator.get_volume_time_log());
}
