#ifndef COMPANY_H
#define COMPANY_H
#include <vector>
#include <utility>

class World;
class Company{
    public:
        Company(World &current_world);
        ~Company();
        std::vector<int> get_all_earnings();
        std::pair<int, float> send_data();
    
    private:
        World &world;
        std::vector<int> earnings;
        float determine_business_cycle(float cycle_value);
        int generate_fcf();
        
}; 

#endif // COMPANY_H