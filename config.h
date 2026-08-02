#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <cmath>

const double pi = std::acos(-1);

//World
constexpr int TICKS_TO_BLOCK = 5;
constexpr int TICKS_PER_CYCLE = 100;
constexpr float RATE_MIN = 0.01;
constexpr float RATE_MAX = 0.15;
constexpr float AMPLITUDE = (RATE_MAX-RATE_MIN)/2.0;
constexpr float MIDPOINT = (RATE_MAX+RATE_MIN)/2.0;

//Agent
constexpr int MAX_TIME_HORIZON = 10;
constexpr int MIN_TIME_HORIZON = 2;
constexpr int GROWTH_RATE_BENCHMARK = 200;

//Company
// Sized so DCF fair value is near INITIAL_PRICE at mid rates / mid growth beliefs
constexpr int BASE_FCF_VALUE = 25000;

//Decision Engine
constexpr float GDP_GROWTH_RATE = 0.03;
constexpr float THRESHOLD = 0.5;

// Agent States
enum class AgentState : char {
    Buying = 'b',
    Selling = 's',
    Holding = 'h',
    Liquid = 'l',
    Uninitialized = 'u'
};

// Order Types
enum class OrderType : uint8_t {
    Buy = 0,
    Sell = 1
};

// Order Status
enum class OrderStatus : uint8_t {
    Active = 0,
    Filled = 1,
    Timeouted = 2,
    Cancelled = 3
};

enum class AgentBehaviour{
    Conservative = 0,
    Agressive = 1
};

// Simulator specific constants
constexpr float RANGE = 0.05f; 
constexpr float BUFFER = 10.0f; // Buffer for price generation
constexpr int NO_AGENTS = 100;
constexpr int TICKS = 1000;
constexpr float INITIAL_PRICE = 100.0f;
constexpr float TOTAL_CASH = 20000000.0f;
constexpr int TOTAL_SHARES = 10000;
constexpr int TIMEOUT_DURATION = 10;
constexpr int NO_BOOK_PRICE_INT = -1;
constexpr float NO_BOOK_PRICE_FLOAT = -1.0f;

// Price representations
// We use 2dp as standard accuracy for everything money related and map floats to integer ticks
constexpr int PRICE_MULTIPLIER = 100; 

// Inline translation functions
inline int float_to_int_price(float price) {
    return static_cast<int>(std::round(price * PRICE_MULTIPLIER));
}

inline float int_to_float_price(int price) {
    return static_cast<float>(price) / PRICE_MULTIPLIER;
}

inline bool is_valid_book_price(int price) {
    return price != NO_BOOK_PRICE_INT;
}

// Log toggles for performance vs real-time debugging 
constexpr bool USE_BUFFERED_LOGGING = true;
constexpr int LOG_BUFFER_SIZE = 100;

#endif // CONFIG_H
