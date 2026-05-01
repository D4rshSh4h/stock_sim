#ifndef SIMULATION_RUNNER_H
#define SIMULATION_RUNNER_H

class Simulator;

class SimulationRunner {
public:
  void run(Simulator &simulator, int ticks, int timeout_duration) const;
};

#endif // SIMULATION_RUNNER_H
