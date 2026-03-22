#ifndef AGENT_H
#define AGENT_H

#include "Simulator.h"
#include "orderbook.h"

class Agent{
    private:
        Simulator& simulator;
        int id;
        int cash;
        int shares;
        char state; // b for buying, s for selling, h for holding, l for perfectly liquid, m or mix of both shares and cash
        Order prediction_engine();
        void place_order(Order& order);
    public:
        Agent() = default;
        Agent(Simulator& sim); //TODO need to define constructor
        ~Agent();
        //current mkt price is a simulator function
        //Changing state will be done in log_trade in trades.cpp, unless order is bieng placed in which case it will be done here
        void run_agent();
        

};

#endif // AGENT_H