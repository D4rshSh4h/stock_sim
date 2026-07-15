#include "World.h"
#include "../config.h"

World::~World() {}

World::World() : freq_multiplier(2.0*pi / TICKS_PER_CYCLE){}

int World::get_time() const {
    return time;
}

void World::update_time(){
    time++;
    if(time % (TICKS_TO_BLOCK * 3) == 0){
        change_interest_rate();
    }
}
     
float World::get_business_cycle(){
    //TODO define what each value means at the company level
    return std::sin(time*freq_multiplier);
}

float World::get_interest_rate(){
    return (AMPLITUDE*::sin((time*freq_multiplier)+pi/2))+MIDPOINT;
}