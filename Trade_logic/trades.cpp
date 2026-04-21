#include "trades.h"
#include "TradeLogger.h"
#include "../config.h"
#include <iostream>
#include <fstream>

MatchingEngine::MatchingEngine(TradeObserver* observer) : observer_(observer) {}

float MatchingEngine::calculate_spread(Order& buy_order, Order& sell_order) {
    return int_to_float_price(buy_order.getPrice() - sell_order.getPrice());
}

bool MatchingEngine::match_orders(Order& order, Orderbook& buy, Orderbook& sell) {
    OrderType trade_type = order.getTradeType();
    int order_price = order.getPrice(); 
    bool matched = false;
    
    if(trade_type == OrderType::Buy){
        while(!(sell.is_empty())){
            auto lowest_sell_opt = sell.best_ask_price();
            if(!lowest_sell_opt.has_value()){
                return false;
            }
            int lowest_sell_price = *lowest_sell_opt;
            if (lowest_sell_price <= order_price){
                auto sell_order = sell.get_order(lowest_sell_price);
                if (!sell_order.has_value()){
                    return false;
                }
                float spread = calculate_spread(order, *sell_order);
                log_trade(order, *sell_order, int_to_float_price(lowest_sell_price), spread);
                auto sell_orders = sell.get_orders_at_price(lowest_sell_price);
                if (sell_orders && !sell_orders->empty()) {
                    sell_orders->front()->fill_order();
                }
                sell.removeOrder(*sell_order);
                matched = true;
                break; // incoming order fully filled, stop
            } else {
                break;
            }
        }
    }

    if(trade_type == OrderType::Sell){
        while(!(buy.is_empty())){
            auto highest_buy_opt = buy.best_bid_price();
            if(!highest_buy_opt.has_value()){
                return false;
            }
            int highest_buy_price = *highest_buy_opt;
            if (highest_buy_price >= order_price){
                auto buy_order = buy.get_order(highest_buy_price);
                if (!buy_order.has_value()){
                    return false;
                }
                float spread = calculate_spread(*buy_order, order);
                log_trade(*buy_order, order, int_to_float_price(highest_buy_price), spread);
                auto buy_orders = buy.get_orders_at_price(highest_buy_price);
                if (buy_orders && !buy_orders->empty()) {
                    buy_orders->front()->fill_order();
                }
                buy.removeOrder(*buy_order);
                matched = true;
                break; // incoming order fully filled, stop
            } else {
                break;
            }
        }
    }
    return matched;
}

//Receives a new incoming order, Decides what to do with it, Calls the matching logic, If unmatched, rests the order in the book
bool MatchingEngine::execute_buy_order(Order& order, Orderbook& buy, Orderbook& sell) { 
    //If trade not matched add to orderbook
    if (!match_orders(order, buy, sell)) {
        std::shared_ptr<Order> order_ptr = buy.addOrder(order);
        if(observer_){
        observer_->track_resting_order_for_timeout(order_ptr);
        }
        else{
            std::cout << "No trade observer set. Unable to update agent states." << std::endl;
        }
        return false;
    }
    return true;
}

bool MatchingEngine::execute_sell_order(Order& order, Orderbook& buy, Orderbook& sell) {
    //If trade not matched add to orderbook
    if (!match_orders(order, buy, sell)) {
        std::shared_ptr<Order> order_ptr = sell.addOrder(order);
        if(observer_){
        observer_->track_resting_order_for_timeout(order_ptr);
        }
        else{
            std::cout << "No trade observer set. Unable to update agent states." << std::endl;
        }
        return false;
    }
    return true;
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

void MatchingEngine::log_trade(Order& buyer, Order& seller, float price, float spread) {
    if(observer_){
        observer_->on_trade_executed(buyer, seller, price, spread);
    }
    else{
        std::cout << "No trade observer set. Unable to update agent states." << std::endl;
    }

    TradeLogger::instance().log(buyer.getId(), seller.getId(), price, spread);
}

void MatchingEngine::sweep_orderbooks(Orderbook& buy, Orderbook& sell) {
    while (!buy.is_empty() && !sell.is_empty()) {
        auto highest_buy_opt = buy.best_bid_price();
        auto lowest_sell_opt = sell.best_ask_price();
        if (!highest_buy_opt.has_value() || !lowest_sell_opt.has_value()) {
            break;
        }
        int highest_buy_price = *highest_buy_opt;
        int lowest_sell_price = *lowest_sell_opt;

        if (highest_buy_price >= lowest_sell_price) {
            auto buy_order = buy.get_order(highest_buy_price);
            auto sell_order = sell.get_order(lowest_sell_price);

            if (!buy_order.has_value() || !sell_order.has_value()) break;

            float spread = calculate_spread(*buy_order, *sell_order);
            float traded_price = int_to_float_price((highest_buy_price + lowest_sell_price) / 2);
            log_trade(*buy_order, *sell_order, traded_price, spread);
            auto buy_orders = buy.get_orders_at_price(highest_buy_price);
            if (buy_orders && !buy_orders->empty()) {
                buy_orders->front()->fill_order();
            }
            auto sell_orders = sell.get_orders_at_price(lowest_sell_price);
            if (sell_orders && !sell_orders->empty()) {
                sell_orders->front()->fill_order();
            }
            buy.removeOrder(*buy_order);
            sell.removeOrder(*sell_order);
        } else {
            break;
        }
    }
}

bool buy_trade(Order &order, Orderbook &buy, Orderbook &sell, TradeObserver& observer) {
    MatchingEngine engine(&observer);
    return engine.execute_buy_order(order, buy, sell);
}

bool sell_trade(Order &order, Orderbook &buy, Orderbook &sell, TradeObserver& observer) {
    MatchingEngine engine(&observer);
    return engine.execute_sell_order(order, buy, sell);
}

void sweep_book(Orderbook& buy, Orderbook& sell, TradeObserver& observer) {
    MatchingEngine engine(&observer);
    engine.sweep_orderbooks(buy, sell);
}

bool MatchingEngine::buy_trade(Order& order, Orderbook& buy, Orderbook& sell) {
    return execute_buy_order(order, buy, sell);
}

bool MatchingEngine::sell_trade(Order& order, Orderbook& buy, Orderbook& sell) {
    return execute_sell_order(order, buy, sell);
}

void MatchingEngine::sweep_book(Orderbook& buy, Orderbook& sell) {
    sweep_orderbooks(buy, sell);
}

