#include "Company.h"
#include "World.h"
#include "../config.h"
#include <cmath>

Company::Company(World &current_world) : world(current_world){
    earnings.push_back(BASE_FCF_VALUE);
}
Company::~Company(){}

float Company::determine_business_cycle(float cycle_value){
    float cos_value = std::cos(cycle_value);
    float sin_value = std::sin(cycle_value);
    if (sin_value >= 0)
    {
        if (cos_value > 0)
        {
        //Decelerating growth
        return cos_value +0.5;
        }
    else
        {
        //Accelerating recession
        return cos_value - 1;
        }
    }
    else
    {
        if (cos_value > 0)
        {
        //Accelerating growth
        return cos_value + 1;
        }
        else
        {
        //Decelerating recession
        return cos_value - 0.5;
        }
    }    

}

int Company::generate_fcf(){
    // Earnings release every 5 blocks (see main.cpp tick structure comments)
    constexpr int earnings_period = TICKS_TO_BLOCK * 5;
    if (world.get_time() % earnings_period != 0) {
        return earnings.back();
    }
    float multiplier = determine_business_cycle(world.get_business_cycle());
    int fcf_final = static_cast<int>(std::round(earnings.back()*multiplier));
    earnings.push_back(fcf_final);
    return fcf_final;

}

std::vector<int> Company::get_all_earnings(){
    return earnings;
}

std::pair<int, float> Company::send_data(){
    std::pair<int, float> p{generate_fcf(), world.get_interest_rate()};
    return p;
}