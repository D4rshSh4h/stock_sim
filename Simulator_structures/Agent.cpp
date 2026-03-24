#include "Agent.h"
#include <iostream>
//#include <stdexcept>

Agent::Agent(Simulator& sim, int id, float cash, int shares, char state) : simulator(sim), id(id), cash(cash), shares(shares), state(state) {}
Agent::~Agent() {}



Order Agent::prediction_engine() {
    float current_price = simulator.get_current_price();
    /*
    This first iteration is a completely random prediction engine. Therefore, the state of the agent will not be used.
    Instead the agent will randomly decide to buy, sell or hold, given the constraints of its cash and shares. 
    Reminder that currently only one share can be traded at a time. 
    0 cash and 0 shares will throw an error (TODO: implement logic to remove agent from sim)
    An agent which has an order placed in an orderbook will have its cash/shares deducted until said order is filled.
    */

    if(cash == 0 && shares == 0){
        throw std::runtime_error("Agent has no cash or shares to trade");
    }
    else if(shares == 0){
        //Only have cash - random chance to buy or do nothing
    }
    else if(cash == 0){
        //Only have shares - random chance to sell or do nothing
    }
    else{
        //Have both cash and shares - random chance to buy, sell or do nothing
    }

}