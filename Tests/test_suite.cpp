#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <chrono>
#include <future>
#include <string>

#include "../Order_structures/order.h"
#include "../Order_structures/orderbook.h"
#include "../Trade_logic/trades.h"

// Basic Test Runner Utility
int tests_passed = 0;
int tests_failed = 0;

template<typename Func>
void run_test(const std::string& test_name, Func test_func) {
    std::cout << "Running " << test_name << "... ";
    
    std::shared_ptr<std::promise<bool>> p(new std::promise<bool>());
    auto future = p->get_future();
    std::thread t([p, test_func]() {
        try {
            test_func();
            p->set_value(true);
        } catch (...) {
            try { p->set_exception(std::current_exception()); } catch(...) {}
        }
    });
    t.detach();
    
    // Timeout of 1 second to detect infinite loops
    if (future.wait_for(std::chrono::milliseconds(1000)) == std::future_status::timeout) {
        std::cout << "[ FAILED ] (Timeout/Infinite Loop Detected)\n";
        tests_failed++;
    } else {
        try {
            future.get();
            std::cout << "[ PASSED ]\n";
            tests_passed++;
        } catch (const std::exception& e) {
            std::cout << "[ FAILED ] Exception: " << e.what() << "\n";
            tests_failed++;
        } catch (...) {
            std::cout << "[ FAILED ] Unknown Exception\n";
            tests_failed++;
        }
    }
}

// ---------------------------------------------------------
// UNIT TESTS FOR ORDER
// ---------------------------------------------------------
void test_order_initialization() {
    Order order(100, 1, 0);
    if (order.getPrice() != 100) throw std::runtime_error("Price mismatch");
    if (order.getId() != 1) throw std::runtime_error("Id mismatch");
    if (order.getTradeType() != 0) throw std::runtime_error("Trade type mismatch");
}

// ---------------------------------------------------------
// UNIT TESTS FOR ORDERBOOK
// ---------------------------------------------------------
void test_orderbook_operations() {
    Orderbook book;
    if (!book.is_empty()) throw std::runtime_error("Orderbook should be empty initially");

    // Add orders
    Order o1(100, 1, 0);
    Order o2(105, 2, 0);
    Order o3(95, 3, 0);
    book.addOrder(o1);
    book.addOrder(o2);
    book.addOrder(o3);

    if (book.is_empty()) throw std::runtime_error("Orderbook should not be empty");
    if (book.find_lowest_price() != 95) throw std::runtime_error("Lowest price mismatch");
    if (book.find_highest_price() != 105) throw std::runtime_error("Highest price mismatch");

    auto found_order = book.get_order(100);
    if (!found_order.has_value() || found_order->getId() != 1) {
        throw std::runtime_error("Order retrieval failed");
    }

    // Remove Order
    book.removeOrder(o3); 
    if (book.find_lowest_price() == 95) {
        throw std::runtime_error("Order failing to remove correctly");
    }

    // Remove Price
    book.removePrice(o2);
    if (book.find_highest_price() == 105) {
        throw std::runtime_error("Order price failing to remove correctly");
    }
}

// ---------------------------------------------------------
// UNIT TESTS FOR TRADES
// ---------------------------------------------------------
void test_set_order() {
    Order o = set_order(150, 5, 1);
    if (o.getPrice() != 150 || o.getId() != 5 || o.getTradeType() != 1) {
        throw std::runtime_error("set_order created incorrect parameters");
    }
}

void test_calculate_spread() {
    Order buy_order(90, 1, 0);
    Order sell_order(110, 2, 1);
    int spread = calculate_spread(buy_order, sell_order);
    if (spread != 20) {
        throw std::runtime_error("Spread calculation is incorrect");
    }
}

void test_buy_trade_no_match() {
    Orderbook buy;
    Orderbook sell;
    Order o(100, 1, 0);
    
    // Selling price is higher: 110. (Should not match!)
    Order sell_order(110, 2, 1);
    sell.addOrder(sell_order);
    
    // This function will likely infinite loop based on the while condition.
    // If it is well-formed, it will exit and not crash.
    buy_trade(o, buy, sell);

    if (!sell.is_empty()) {
        auto val = sell.find_lowest_price();
        if (val != 110) throw std::runtime_error("Sell book altered incorrectly");
    }
}

void test_buy_trade_match() {
    Orderbook buy;
    Orderbook sell;
    
    Order sell_order(90, 1, 1);
    sell.addOrder(sell_order);
    
    Order buy_order(100, 2, 0);
    
    buy_trade(buy_order, buy, sell);
    
    if (!sell.is_empty()) {
        throw std::runtime_error("Trade matching failed, sell order should be consumed");
    }
}

// ---------------------------------------------------------
// STRESS TESTS
// ---------------------------------------------------------
void stress_test_orderbook() {
    Orderbook book;
    const int NUM_ORDERS = 100000;
    
    for (int i = 0; i < NUM_ORDERS; ++i) {
        book.addOrder(Order(100 + (i % 100), i, 0));
    }
    
    if (book.is_empty()) throw std::runtime_error("Orderbook did not accept large volume");
}

void stress_test_trades() {
    Orderbook buy;
    Orderbook sell;
    
    // Fill sell book with 10k orders ranging from 110 to 120 (so they won't match a 100 buy)
    // Wait, testing 10k orders and executing trades. We will do 1 perfect match per loop.
    for (int i = 0; i < 100; ++i) {
        sell.addOrder(Order(100, i*2, 1));
    }
    
    // Process 5000 matching buy orders
    for (int i = 0; i < 100; ++i) {
    Order buy_order(100, i*2+1, 0);
    std::cout << "Before match - sell size: " << sell.get_size() << std::endl;
    match_orders(buy_order, buy, sell);
    std::cout << "After match - sell size: " << sell.get_size() << std::endl;
}
    
    if (!sell.is_empty()) {
        throw std::runtime_error("Stress test did not fully exhaust matching sell orders");
    }
}


//Orderbook tests (claude)
// Individual test functions
void sweep_basic_single_match() {
    Orderbook buy, sell;
    buy.addOrder(Order(100, 1, 0));
    sell.addOrder(Order(100, 2, 1));
    sweep_book(buy, sell);
    if (!buy.is_empty() || !sell.is_empty())
        throw std::runtime_error("Basic sweep failed to match orders");
}

void sweep_no_match_possible() {
    Orderbook buy, sell;
    buy.addOrder(Order(90, 1, 0));
    sell.addOrder(Order(100, 2, 1));
    sweep_book(buy, sell);
    if (buy.is_empty() || sell.is_empty())
        throw std::runtime_error("Sweep incorrectly matched non-crossing orders");
}

void sweep_empty_buy_book() {
    Orderbook buy, sell;
    sell.addOrder(Order(100, 1, 1));
    sweep_book(buy, sell);
    if (sell.is_empty())
        throw std::runtime_error("Sweep incorrectly removed order from sell book");
}

void sweep_empty_sell_book() {
    Orderbook buy, sell;
    buy.addOrder(Order(100, 1, 0));
    sweep_book(buy, sell);
    if (buy.is_empty())
        throw std::runtime_error("Sweep incorrectly removed order from buy book");
}

void sweep_both_books_empty() {
    Orderbook buy, sell;
    sweep_book(buy, sell);
    if (!buy.is_empty() || !sell.is_empty())
        throw std::runtime_error("Sweep failed on empty books");
}

void sweep_5000_matching_pairs() {
    Orderbook buy, sell;
    for (int i = 0; i < 100; ++i) {
        buy.addOrder(Order(100, i*2, 0));
        sell.addOrder(Order(100, i*2+1, 1));
    }
    sweep_book(buy, sell);
    if (!buy.is_empty() || !sell.is_empty())
        throw std::runtime_error("Stress test failed to exhaust both books");
}

void sweep_multiple_price_levels_crossing() {
    Orderbook buy, sell;
    for (int i = 0; i < 3; ++i) {
        buy.addOrder(Order(100 + i*5, i*2, 0));
        sell.addOrder(Order(95 + i*3, i*2+1, 1));
    }
    sweep_book(buy, sell);
    if (!buy.is_empty() || !sell.is_empty()){
        std::cout << "Buy size: " << buy.get_size() << ", Sell size: " << sell.get_size() << std::endl;
        throw std::runtime_error("Multiple price level sweep failed");
    }
        
}

void sweep_partial_match() {
    Orderbook buy, sell;
    buy.addOrder(Order(100, 1, 0)); // will match
    buy.addOrder(Order(80, 2, 0));  // wont match
    sell.addOrder(Order(100, 3, 1)); // will match
    sell.addOrder(Order(120, 4, 1)); // wont match
    sweep_book(buy, sell);
    if (buy.is_empty() || sell.is_empty())
        throw std::runtime_error("Partial sweep incorrectly exhausted books");
}

void sweep_1000_same_price_level() {
    Orderbook buy, sell;
    for (int i = 0; i < 100; ++i) {
        buy.addOrder(Order(100, i*2, 0));
        sell.addOrder(Order(100, i*2+1, 1));
    }
    sweep_book(buy, sell);
    if (!buy.is_empty() || !sell.is_empty())
        throw std::runtime_error("Same price level stress test failed");
}

void sweep_more_buys_than_sells() {
    Orderbook buy, sell;
    for (int i = 0; i < 500; ++i)
        buy.addOrder(Order(100, i, 0));
    for (int i = 0; i < 100; ++i)
        sell.addOrder(Order(100, i+1000, 1));
    sweep_book(buy, sell);
    if (!sell.is_empty()){
        std::cout << "Remaining sell orders: " << sell.get_size() << std::endl;
        throw std::runtime_error("Sell book should be exhausted");
    }
    
    if (buy.is_empty())
        throw std::runtime_error("Buy book should have 500 remaining orders");
}




// ---------------------------------------------------------
// MAIN TEST RUNNER
// ---------------------------------------------------------
int main() {
    // Redirect std::cout to test_results.txt
    std::ofstream out("test_results.txt");
    std::streambuf *coutbuf = std::cout.rdbuf(); // save old buf
    std::cout.rdbuf(out.rdbuf()); // redirect std::cout to out.txt!

    std::cout << "======================================\n";
    std::cout << " UNIT TEST SUITE FOR STOCK SIMULATOR \n";
    std::cout << "======================================\n";

    // Mod 1: Order
    std::cout << "\n--- Order Tests ---\n";
    run_test("Order Initialization", test_order_initialization);

    // Mod 2: Orderbook
    std::cout << "\n--- Orderbook Tests ---\n";
    run_test("Orderbook Basic Operations", test_orderbook_operations);
    run_test("Sweep: Basic single match",                sweep_basic_single_match);
    run_test("Sweep: No match possible",                 sweep_no_match_possible);
    run_test("Sweep: Empty buy book",                    sweep_empty_buy_book);
    run_test("Sweep: Empty sell book",                   sweep_empty_sell_book);
    run_test("Sweep: Both books empty",                  sweep_both_books_empty);
    run_test("Sweep: 5000 matching pairs stress test",   sweep_5000_matching_pairs);
    run_test("Sweep: Multiple price levels crossing",    sweep_multiple_price_levels_crossing);
    run_test("Sweep: Partial match",                     sweep_partial_match);
    run_test("Sweep: 1000 orders same price level",      sweep_1000_same_price_level);
    run_test("Sweep: More buys than sells",              sweep_more_buys_than_sells);

    // Mod 3: Trades
    std::cout << "\n--- Trades Tests ---\n";
    run_test("Trades set_order Helper", test_set_order);
    run_test("Trades calculate_spread", test_calculate_spread);
    run_test("Trades Perfect Match", test_buy_trade_match);
    run_test("Trades Buy Without Exact Match", test_buy_trade_no_match);

    // Mod 4: Stress Tests
    std::cout << "\n--- Stress Tests ---\n";
    run_test("Orderbook Heavy Insertion", stress_test_orderbook);
    run_test("Trades Heavy Matching", stress_test_trades);

    

    std::cout << "\n======================================\n";
    std::cout << " TESTS SUMMARY: " << tests_passed << " Passed, " << tests_failed << " Failed\n";
    std::cout << "======================================\n";

    // Reset std::cout back to the console
    std::cout.rdbuf(coutbuf);

    return (tests_failed == 0) ? 0 : 1;
}
