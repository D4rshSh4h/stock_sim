#include "trades.h"
#include "TradeLogger.h"
#include "../config.h"
#include <iostream>
#include <fstream>



TradeObserver* observer_ = nullptr;
void set_trade_observer(TradeObserver* observer) {
    observer_ = observer;
}

float calculate_spread(Order& buy_order, Order& sell_order) {
    return int_to_float_price(buy_order.getPrice() - sell_order.getPrice());
}

// Gets the best resting order from the opposing book as a shared_ptr (live reference)
std::shared_ptr<Order> get_best_resting_order(Orderbook& book, int price, bool is_buy) {
    auto orders = book.get_orders_at_price(price);
    if (!orders || orders->empty()) return nullptr;
    auto order = orders->front();
    if (order->getStatus() != OrderStatus::Active) return nullptr;
    return order;
}

// Handles spread calculation and logging — common to all three scenarios
void process_trade(Order& incoming, std::shared_ptr<Order> resting, float exec_price, int traded_qty) {
    float spread = (incoming.getTradeType() == OrderType::Buy)
        ? calculate_spread(incoming, *resting)
        : calculate_spread(*resting, incoming);
    log_trade(incoming, *resting, exec_price, spread, traded_qty);
}

bool match_orders(Order& order, Orderbook& buy, Orderbook& sell) {
    OrderType trade_type = order.getTradeType();
    int order_price = order.getPrice();
    bool matched = false;

    if (trade_type == OrderType::Buy) {
        while (!sell.is_empty()) {
            auto lowest_sell_opt = sell.best_ask_price();
            if (!lowest_sell_opt.has_value()) return false;
            int lowest_sell_price = *lowest_sell_opt;
            if (lowest_sell_price > order_price) break;

            auto resting = get_best_resting_order(sell, lowest_sell_price, false);
            if (!resting) return false;

            int incoming_qty = order.getQty();
            int resting_qty = resting->getQty();
            int traded_qty = std::min(incoming_qty, resting_qty);
            float exec_price = int_to_float_price(lowest_sell_price);

            if (incoming_qty < resting_qty) {
                // Scenario 2: incoming fully filled, resting stays with reduced qty
                process_trade(order, resting, exec_price, traded_qty);
                resting->changeQty(-traded_qty);
                matched = true;
                break;
            } else if (incoming_qty > resting_qty) {
                // Scenario 1: resting fully filled, loop continues
                process_trade(order, resting, exec_price, traded_qty);
                resting->fill_order();
                sell.removeOrder(*resting);
                order.changeQty(-traded_qty);
                matched = true;
                // continue loop
            } else {
                // Scenario 3: both fully filled
                process_trade(order, resting, exec_price, traded_qty);
                resting->fill_order();
                sell.removeOrder(*resting);
                matched = true;
                break;
            }
        }
    }

    if (trade_type == OrderType::Sell) {
        while (!buy.is_empty()) {
            auto highest_buy_opt = buy.best_bid_price();
            if (!highest_buy_opt.has_value()) return false;
            int highest_buy_price = *highest_buy_opt;
            if (highest_buy_price < order_price) break;

            auto resting = get_best_resting_order(buy, highest_buy_price, true);
            if (!resting) return false;

            int incoming_qty = order.getQty();
            int resting_qty = resting->getQty();
            int traded_qty = std::min(incoming_qty, resting_qty);
            float exec_price = int_to_float_price(highest_buy_price);

            if (incoming_qty < resting_qty) {
                process_trade(order, resting, exec_price, traded_qty);
                resting->changeQty(-traded_qty);
                matched = true;
                break;
            } else if (incoming_qty > resting_qty) {
                process_trade(order, resting, exec_price, traded_qty);
                resting->fill_order();
                buy.removeOrder(*resting);
                order.changeQty(-traded_qty);
                matched = true;
            } else {
                process_trade(order, resting, exec_price, traded_qty);
                resting->fill_order();
                buy.removeOrder(*resting);
                matched = true;
                break;
            }
        }
    }

    return matched;
}


void prepare_file(void) { 
    TradeLogger::instance().init("trade_log.csv");

    std::ofstream log_volume_file("volume_log.csv", std::ios::trunc);
    if (log_volume_file.is_open()) {
        log_volume_file << "Time, Volume" << std::endl;
        log_volume_file.close();
    } else {
        std::cout << "Unable to initialize volume log file." << std::endl;
    }
}

void log_trade(Order& buyer, Order& seller, float price, float spread, int qty) {
    if(observer_){
        observer_->on_trade_agent_state(buyer, seller, price, spread, qty);
    }
    else{
        std::cout << "No trade observer set. Unable to update agent states." << std::endl;
    }

    TradeLogger::instance().log(buyer.getId(), seller.getId(), price, spread, qty);
}



bool buy_trade(Order &order, Orderbook &buy, Orderbook &sell, TradeObserver& observer) {
    //If trade not matched add to orderbook
    if (!match_orders(order, buy, sell)) {
        std::shared_ptr<Order> order_ptr = buy.addOrder(order);
        if(observer_){
        observer_->update_time_order_index(order_ptr);
        }
        else{
            std::cout << "No trade observer set. Unable to update agent states." << std::endl;
        }
        return false;
    }
    return true;
}

bool sell_trade(Order &order, Orderbook &buy, Orderbook &sell, TradeObserver& observer) {
    //If trade not matched add to orderbook
    if (!match_orders(order, buy, sell)) {
        std::shared_ptr<Order> order_ptr = sell.addOrder(order);
        if(observer_){
        observer_->update_time_order_index(order_ptr);
        }
        else{
            std::cout << "No trade observer set. Unable to update agent states." << std::endl;
        }
        return false;
    }
    return true;
}

void sweep_book(Orderbook& buy, Orderbook& sell, TradeObserver& observer) {
    while (!buy.is_empty() && !sell.is_empty()) {
        auto highest_buy_opt = buy.best_bid_price();
        auto lowest_sell_opt = sell.best_ask_price();
        if (!highest_buy_opt.has_value() || !lowest_sell_opt.has_value()) {
            break;
        }
        int highest_buy_price = *highest_buy_opt;
        int lowest_sell_price = *lowest_sell_opt;

        if (highest_buy_price >= lowest_sell_price) {
            
            auto buy_orders = buy.get_orders_at_price(highest_buy_price);
            auto sell_orders = sell.get_orders_at_price(lowest_sell_price);
            if (!buy_orders || buy_orders->empty()) break;
            if (!sell_orders || sell_orders->empty()) break;
            auto buy_order = buy_orders->front();
            if(buy_order->getStatus() != OrderStatus::Active){
                buy.removeOrder(*buy_order);
                continue;
            }
            auto sell_order = sell_orders->front();
            if(sell_order->getStatus() != OrderStatus::Active){
                sell.removeOrder(*sell_order);
                continue;
            }
            int buy_qty = buy_order->getQty();
            int sell_qty = sell_order->getQty();
            int book_traded_qty = std::min(buy_qty, sell_qty);
            float traded_price = int_to_float_price((highest_buy_price + lowest_sell_price) / 2);
            float spread = calculate_spread(*buy_order, *sell_order); 

            if(buy_qty > sell_qty){
                sell_order->fill_order();
                sell.removeOrder(*sell_order);
                buy_order->changeQty(-book_traded_qty);
                log_trade(*buy_order, *sell_order, traded_price, spread, book_traded_qty); 
                continue;
            } else if (buy_qty < sell_qty){
                buy_order->fill_order();
                buy.removeOrder(*buy_order);
                sell_order->changeQty(-book_traded_qty);
                log_trade(*buy_order, *sell_order, traded_price, spread, book_traded_qty); 
                continue;
            }
            else{
                sell_order->fill_order();
                buy_order->fill_order();
                buy.removeOrder(*buy_order);
                sell.removeOrder(*sell_order);
                log_trade(*buy_order, *sell_order, traded_price, spread, book_traded_qty);
            }
            
        } else {
            break;
        }
    }
   
}



