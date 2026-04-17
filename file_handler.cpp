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


