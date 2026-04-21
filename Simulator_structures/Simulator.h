#ifndef SIMULATOR_H
#define SIMULATOR_H 


#include "../Order_structures/orderbook.h"
#include "DecisionEngine.h"
#include "TradeObserver.h"
#include <unordered_map>
#include <memory>
#include <queue>

class Agent;
 
class Simulator : public TradeObserver {
    public:
        Simulator(float initial_price = 100.0, float total_cash = 100000.0, int total_shares = 1000); //Constructor with default values for initial price, total cash and total shares
        Simulator() = default;
        ~Simulator();
        float get_current_price() const;
        int get_volume(int time) const; //Gets a single volume
        float get_price(int time) const; //Gets a single price
        void update_time(); 
        void log_price(); //Adds current price to price_time_log with current time as key
        void log_volume(); //Adds current volume to volume_time_log with current time as key
        void update_price(float lowest_ask, float highest_bid);
        Orderbook& get_buy_book();
        Orderbook& get_sell_book(); 
        Agent* get_agent(int id);
        void simulator_buy_trade(Order& order);
        void simulator_sell_trade(Order& order); 
        void on_trade_executed(const Order& buy_order, const Order& sell_order, float price, float spread) override;
        void track_resting_order_for_timeout(std::shared_ptr<Order> order_ptr) override;
        // Compatibility wrappers for existing names.
        void on_trade_agent_state(const Order& buy_order, const Order& sell_order, float price, float spread) override;
        void update_time_order_index(std::shared_ptr<Order> order_ptr) override;
        void create_vector_agent_ids(); 
        std::vector<int> shuffle_agent_ids();
        void reset_volume();
        std::map<int, int> get_volume_time_log();
        const std::map<int, int>& get_volume_time_log_ref() const;
        int get_current_time() const;
        void expire_timed_out_orders(int timeout_duration = 5);
        void find_order_timeouts(int timeout_duration = 5);
        void initialize_agents(int no_agents = 10);
        void simulator_start(int no_agents = 10); // compatibility wrapper
        
    private:
        float current_price;
        int volume;
        int time;
        float total_cash; //Total cash circulating in system
        int total_shares; //Total shares circulating in system
        Orderbook buy_book;
        Orderbook sell_book;
        std::map<int, int> volume_time_log; // maps time to volume for that time step <int time, int volume>
        std::map<int, float> price_time_log; // maps time to price for that time step <int time, float price>
        
        std::vector<int> agent_ids; // Vector to store agent IDs for random iteration
        std::unordered_map<int, std::unique_ptr<Agent>> agents; // maps agent id to agent object <int id, Agent agent>
        std::queue<std::weak_ptr<Order>> timeout_queue;
        RandomDecisionEngine random_decision_engine;

        size_t choose_split_index(int no_agents);
        const IAgentDecisionEngine& select_engine_for_agent(int agent_id) const;
        bool allocate_cash_agents(std::vector<std::unique_ptr<Agent>>& temp_agent_vector, int no_cash_agents);
        void allocate_share_agents(std::vector<std::unique_ptr<Agent>>& temp_agent_vector, int no_agents, size_t split_index);
        void register_agents(std::vector<std::unique_ptr<Agent>>& temp_agent_vector);

};

#endif // SIMULATOR_H