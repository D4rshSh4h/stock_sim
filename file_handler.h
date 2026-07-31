#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <map>
#include <vector>
#include <string>

class Simulator;

void prepare_extra_logs();
void print_volume(const std::map<int, int>& volume_map);
void print_cycle(const std::vector<float>& business_cycle, const std::vector<float>& interest_rates);

void log_tick(int time,
              float last_price,
              int volume,
              float best_bid,
              float best_ask,
              int buy_levels,
              int sell_levels,
              int buy_qty,
              int sell_qty,
              int fcf,
              float interest_rate,
              float business_cycle);

void log_order(int time,
               int agent_id,
               int side, // 0=buy, 1=sell
               float price,
               int qty,
               int growth_rate,
               int behaviour,
               int horizon,
               float cash,
               int shares,
               float mkt_price);

void log_agent_snapshots(const Simulator& sim, int time, float mark_price);

#endif // FILE_HANDLER_H
