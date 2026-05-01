#ifndef SIMULATION_OUTPUT_SINK_H
#define SIMULATION_OUTPUT_SINK_H

#include <map>

class SimulationOutputSink {
public:
  void prepare() const;
  void finalize(const std::map<int, int> &volume_time_log) const;
};

#endif // SIMULATION_OUTPUT_SINK_H
