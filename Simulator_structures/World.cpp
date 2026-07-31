#include "World.h"
#include "../config.h"
#include <iostream>

World::~World() {}

World::World() : freq_multiplier(2.0*pi / TICKS_PER_CYCLE){
    change_interest_rate(); // valid starting rate before first scheduled update
}

int World::get_time() const {
    return time;
}

void World::change_interest_rate(){
    //For now just using the sin wave as a simple placeholder
    interest_rate = (AMPLITUDE*std::sin((time*freq_multiplier)+pi/2))+MIDPOINT;
}

void World::update_time(){
    time++;
    if(time % (TICKS_TO_BLOCK * 3) == 0){
        //std::cout << "change rate called at time: " << time << std::endl;
        change_interest_rate();
    }
    business_cycle_map.push_back(std::sin(get_business_cycle()));
    interest_rate_map.push_back(interest_rate);
}

//Returns ANGLE not the sin(value)
float World::get_business_cycle(){
    //TODO define what each value means at the company level
    return time*freq_multiplier;
}

float World::get_interest_rate(){
    return std::round(interest_rate*100)/100;             
}  
 
std::vector<float> World::get_business_cycle_map(){
    return business_cycle_map;
}

std::vector<float> World::get_interest_rate_map(){
    return interest_rate_map;
}
