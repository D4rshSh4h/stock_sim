#ifndef WORLD_H
#define WORLD_H

class World{
    public:
        World() = default;
        ~World();
        int get_time() const;
        void update_time();
    
    private:
        int time;
};

#endif // WORLD_H