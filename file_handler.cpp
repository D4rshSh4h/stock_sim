#include "file_handler.h"
#include "Simulator_structures/Simulator.h"
#include "Simulator_structures/Agent.h"
#include "config.h"
#include <iostream>
#include <fstream>

namespace {
std::ofstream tick_file;
std::ofstream order_file;
std::ofstream agent_file;
bool agent_header_written = false;
}

void prepare_extra_logs() {
    tick_file.open("tick_log.csv", std::ios::out | std::ios::trunc);
    if (tick_file.is_open()) {
        tick_file << "Time, LastPrice, Volume, BestBid, BestAsk, Spread, Mid,"
                  << " BuyLevels, SellLevels, BuyQty, SellQty, FCF, InterestRate, BusinessCycle"
                  << std::endl;
    } else {
        std::cout << "Unable to initialize tick_log.csv" << std::endl;
    }

    order_file.open("order_log.csv", std::ios::out | std::ios::trunc);
    if (order_file.is_open()) {
        order_file << "Time, AgentID, Side, Price, Qty, GrowthRate, Behaviour, Horizon,"
                   << " Cash, Shares, MktPrice"
                   << std::endl;
    } else {
        std::cout << "Unable to initialize order_log.csv" << std::endl;
    }

    agent_file.open("agent_log.csv", std::ios::out | std::ios::trunc);
    agent_header_written = false;
    if (!agent_file.is_open()) {
        std::cout << "Unable to initialize agent_log.csv" << std::endl;
    }
}

void print_volume(const std::map<int, int>& volume_map) {
    std::ofstream log_file("volume_log.csv", std::ios::app);
    if (log_file.is_open()) {
        for (const auto& pair : volume_map) {
            log_file << pair.first << ", " << pair.second << std::endl;
        }
        log_file.close();
    } else {
        std::cout << "Unable to initialize volume log file." << std::endl;
    }
}

void print_cycle(const std::vector<float>& business_cycle, const std::vector<float>& interest_rates){
    std::ofstream cycle_file("cycle_log.csv", std::ios::trunc);
    if (cycle_file.is_open()) {
        cycle_file << "Time, BusinessCycle, InterestRate" << std::endl;
        for (size_t i = 0; i < business_cycle.size() && i < interest_rates.size(); ++i) {
            cycle_file << (i + 1) << ", " << business_cycle[i] << ", " << interest_rates[i] << std::endl;
        }
        cycle_file.close();
    } else {
        std::cout << "Unable to initialize cycle log file." << std::endl;
    }
}

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
              float business_cycle) {
    if (!tick_file.is_open()) return;

    float spread = NO_BOOK_PRICE_FLOAT;
    float mid = NO_BOOK_PRICE_FLOAT;
    const bool has_bid = best_bid != NO_BOOK_PRICE_FLOAT;
    const bool has_ask = best_ask != NO_BOOK_PRICE_FLOAT;
    if (has_bid && has_ask) {
        spread = best_ask - best_bid;
        mid = (best_ask + best_bid) / 2.0f;
    } else if (has_bid) {
        mid = best_bid;
    } else if (has_ask) {
        mid = best_ask;
    }

    tick_file << time << ", "
              << last_price << ", "
              << volume << ", "
              << best_bid << ", "
              << best_ask << ", "
              << spread << ", "
              << mid << ", "
              << buy_levels << ", "
              << sell_levels << ", "
              << buy_qty << ", "
              << sell_qty << ", "
              << fcf << ", "
              << interest_rate << ", "
              << business_cycle
              << std::endl;
}

void log_order(int time,
               int agent_id,
               int side,
               float price,
               int qty,
               int growth_rate,
               int behaviour,
               int horizon,
               float cash,
               int shares,
               float mkt_price) {
    if (!order_file.is_open()) return;
    order_file << time << ", "
               << agent_id << ", "
               << side << ", "
               << price << ", "
               << qty << ", "
               << growth_rate << ", "
               << behaviour << ", "
               << horizon << ", "
               << cash << ", "
               << shares << ", "
               << mkt_price
               << std::endl;
}

void log_agent_snapshots(const Simulator& sim, int time, float mark_price) {
    if (!agent_file.is_open()) return;
    if (!agent_header_written) {
        agent_file << "Time, AgentID, Behaviour, GrowthRate, Horizon, Cash, Shares, State, Wealth"
                   << std::endl;
        agent_header_written = true;
    }

    for (int id : sim.get_agent_ids()) {
        const Agent* agent = sim.get_agent(id);
        if (!agent) continue;
        float wealth = agent->get_cash() + agent->get_shares() * mark_price;
        agent_file << time << ", "
                   << agent->get_id() << ", "
                   << static_cast<int>(agent->get_behaviour()) << ", "
                   << agent->get_growth_rate() << ", "
                   << agent->get_time_horizon() << ", "
                   << agent->get_cash() << ", "
                   << agent->get_shares() << ", "
                   << static_cast<char>(agent->get_state()) << ", "
                   << wealth
                   << std::endl;
    }
}
