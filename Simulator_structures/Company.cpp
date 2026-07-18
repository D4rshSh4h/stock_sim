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
    float multiplier = determine_business_cycle(world.get_business_cycle());
    int fcf_final = static_cast<int>(std::round(earnings.back()*multiplier));
    earnings.push_back(fcf_final);
    return fcf_final;

}

std::vector<int> Company::get_all_earnings(){
    return earnings;
}