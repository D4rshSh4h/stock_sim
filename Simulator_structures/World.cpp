#include "World.h"
#include "../config.h"

World::~World() {}

int World::get_time() const {
    return time;
}

void World::update_time(){
    time++;
    if(((time/TICKS_TO_BLOCK) % 3) == 0){
        change_interest_rate();
    }
}