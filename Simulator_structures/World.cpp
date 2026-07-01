#include "World.h"

World::~World() {}

int World::get_time() const {
    return time;
}

void World::update_time(){
    time++;
}