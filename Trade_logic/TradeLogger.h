#ifndef TRADELOGGER_H
#define TRADELOGGER_H

#include "../config.h"
#include <fstream>
#include <string>
#include <vector>

/*
 * TradeLogger — singleton that manages writing trade records to CSV.
 *
 * Two modes (toggled by USE_BUFFERED_LOGGING in config.h):
 *   Buffered  : rows are accumulated in memory and flushed to disk in batches
 *               of LOG_BUFFER_SIZE.  The file handle is kept open for the
 *               lifetime of the logger, avoiding repeated open/close overhead.
 *   Immediate : each record is written synchronously (original behaviour),
 *               useful during debugging.
 *
 * Usage:
 *   TradeLogger::instance().init("trade_log.csv");
 *   TradeLogger::instance().log(buyer_id, seller_id, price, spread);
 *   TradeLogger::instance().flush();   // call at end of simulation
 */
class TradeLogger {
public:
    // Returns the singleton instance
    static TradeLogger& instance();

    // Open (or truncate) the output file and write the CSV header.
    // Must be called once before any log() calls.
    void init(const std::string& filepath);

    // Record one trade.
    void log(int buyer_id, int seller_id, float price, float spread, int qty);

    // Force-flush any remaining buffered rows to disk.
    // Should be called when the simulation ends.
    void flush();

    // Destructor flushes and closes cleanly.
    ~TradeLogger();

private:
    TradeLogger() = default;
    TradeLogger(const TradeLogger&) = delete;
    TradeLogger& operator=(const TradeLogger&) = delete;

    // Buffered mode internals
    struct TradeRecord {
        int   buyer_id;
        int   seller_id;
        float price;
        float spread;
        int qty;
    };

    std::vector<TradeRecord> buffer_;   // in-memory accumulator
    std::ofstream            file_;     // persistent file handle (buffered mode)
    std::string              filepath_; // stored for immediate-mode reopening

    void flush_buffer_to_file();        // writes buffer_ to file_ and clears it
};

#endif // TRADELOGGER_H
