#ifndef WORLD_H
#define WORLD_H
#include <vector>

class World{
    public:
       // World() = default;
        World();
        ~World();
        int get_time() const;
        void update_time();
        float get_interest_rate();
        float get_business_cycle();
        std::vector<float> get_business_cycle_map();
        std::vector<float> get_interest_rate_map();
    
    private:
        void change_interest_rate();
        int time = 0; 
        float interest_rate;
        float freq_multiplier;
        std::vector<float> business_cycle_map;
        std::vector<float> interest_rate_map;

};

#endif // WORLD_H 