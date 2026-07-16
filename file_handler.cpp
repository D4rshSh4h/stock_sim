#include <iostream>
#include <fstream>
#include "file_handler.h"

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
        cycle_file << "Business Cycle, Interest Rates" << std::endl;
        for (size_t i = 0; i < business_cycle.size() && i < interest_rates.size(); ++i) {
            cycle_file << business_cycle[i] << ", " << interest_rates[i] << std::endl;
        }
        cycle_file.close();
    } else {
        std::cout << "Unable to initialize cycle log file." << std::endl;
    }
}



