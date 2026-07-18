#ifndef COMPANY_H
#define COMPANY_H
#include <vector>

class World;
class Company{
    public:
        Company(World &current_world);
        ~Company();
        int generate_fcf();
        std::vector<int> get_all_earnings();
    
    private:
        World &world;
        std::vector<int> earnings;
        float determine_business_cycle(float cycle_value);
        
}; 

#endif // COMPANY_H