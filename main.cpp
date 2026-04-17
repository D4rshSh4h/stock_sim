#include "file_handler.h"
#include "Simulator_structures/Agent.h"
#include "Trade_logic/trades.h"
#include <iostream>

//Constants
const int NO_AGENTS = 50;
const int TICKS = 1000;
const float INITIAL_PRICE = 100.0;
const float TOTAL_CASH = 200000.0;
const int TOTAL_SHARES = 1000;
const int TIMEOUT_DURATION = 10; // Time steps after which an order will timeout if not filled
//TODO need to create a mechanism to add cash mid simulation

int main(){
    //Setup
    Simulator simulator(INITIAL_PRICE, TOTAL_CASH, TOTAL_SHARES);
    set_trade_observer(&simulator); 
    prepare_file();
    simulator.simulator_start(NO_AGENTS);
    std::cout << "Finished prep starting sim" << std::endl;

    Orderbook& buy_book_ref = simulator.get_buy_book();
    Orderbook& sell_book_ref = simulator.get_sell_book();

    //Simulation
    //TODO track bid-ask prices
    for(int i = 0; i<TICKS; i++){
        simulator.update_time();
        std::vector<int> shuffled_ids = simulator.shuffle_agent_ids();
        for(int id : shuffled_ids){
            Agent* agent = simulator.get_agent(id);
            if(agent){
                agent->run_agent();
            }
        }
        
        sweep_book(buy_book_ref, sell_book_ref);
        simulator.find_order_timeouts(TIMEOUT_DURATION);
        
        //simulator.log_price();
        simulator.log_volume();
        simulator.reset_volume();
    }
    print_volume(simulator.get_volume_time_log());
}



