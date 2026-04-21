#include "TradeLogger.h"
#include <iostream>

TradeLogger& TradeLogger::instance() {
    static TradeLogger instance;
    return instance;
}

void TradeLogger::init(const std::string& filepath) {
    filepath_ = filepath;
    if (USE_BUFFERED_LOGGING) {
        file_.open(filepath_, std::ios::out | std::ios::trunc);
        if (file_.is_open()) {
            file_ << "Buyer ID, Seller ID, Price, Spread" << std::endl;
        } else {
            std::cerr << "TradeLogger: Unable to open file " << filepath_ << std::endl;
        }
    } else {
        // Immediate mode simply prepares the file by writing the header
        std::ofstream header_file(filepath_, std::ios::out | std::ios::trunc);
        if (header_file.is_open()) {
            header_file << "Buyer ID, Seller ID, Price, Spread" << std::endl;
        } else {
            std::cerr << "TradeLogger: Unable to open file " << filepath_ << std::endl;
        }
    }
}

void TradeLogger::log(int buyer_id, int seller_id, float price, float spread) {
    if (USE_BUFFERED_LOGGING) {
        buffer_.push_back({buyer_id, seller_id, price, spread});
        if (buffer_.size() >= LOG_BUFFER_SIZE) {
            flush_buffer_to_file();
        }
    } else {
        // Immediate mode (legacy/debug behavior)
        std::ofstream log_file(filepath_, std::ios::app);
        if (log_file.is_open()) {
            log_file << buyer_id << ", " << seller_id << ", " << price << ", " << spread << std::endl;
        }
    }
}

void TradeLogger::flush() {
    if (USE_BUFFERED_LOGGING) {
        flush_buffer_to_file();
        if (file_.is_open()) {
            file_.flush();
        }
    }
}

void TradeLogger::flush_buffer_to_file() {
    if (buffer_.empty()) return;

    if (file_.is_open()) {
        for (const auto& record : buffer_) {
            file_ << record.buyer_id << ", " 
                  << record.seller_id << ", " 
                  << record.price << ", " 
                  << record.spread << std::endl;
        }
        buffer_.clear();
    } else {
        std::cerr << "TradeLogger: File not open for flushing" << std::endl;
    }
}

TradeLogger::~TradeLogger() {
    flush();
    if (file_.is_open()) {
        file_.close();
    }
}
