#ifndef WORLD_H
#define WORLD_H

class World{
    public:
        World() = default;
        ~World();
        int get_time() const;
        void update_time();
        float get_interest_rate();
        int get_business_cycle();
    
    private:
        void change_interest_rate();
        int time;
        float interest_rate;
};

#endif // WORLD_H