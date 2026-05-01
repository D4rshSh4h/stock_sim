#include "SimulationOutputSink.h"
#include "../Trade_logic/TradeLogger.h"
#include "../Trade_logic/trades.h"
#include "../file_handler.h"

void SimulationOutputSink::prepare() const { prepare_file(); }

void SimulationOutputSink::finalize(const std::map<int, int> &volume_time_log) const {
  TradeLogger::instance().flush();
  print_volume(volume_time_log);
}
