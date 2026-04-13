#include "../Order_structures/orderbook.h"
#include "../Simulator_structures/TradeObserver.h"
// Returns an order object with the given price and id
Order set_order(float price, int id, int type);

// Returns true if the order matches with an order in the orderbook, false
// otherwise. Function is called immideately after a new order is called Takes
// liquidity from the market If false, then order is added to the orderbook
bool buy_trade(Order &order, Orderbook &buy, Orderbook &sell);
bool sell_trade(Order &order, Orderbook &buy, Orderbook &sell);

// Runs the matching engine, which matches orders in the orderbook and executes
// trades
bool match_orders(Order &order, Orderbook &buy, Orderbook &sell);
void sweep_book(Orderbook& buy, Orderbook& sell);

void prepare_file(void);
void log_trade(Order &buyer, Order &seller, float price, float spread);
float calculate_spread(Order &buy_order, Order &sell_order);

//void update_agent_state(Order& buyer, Order& seller, float price);

void set_trade_observer(TradeObserver* observer);