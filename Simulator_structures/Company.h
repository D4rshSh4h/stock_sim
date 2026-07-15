#ifndef COMPANY_H
#define COMPANY_H

class Company{
    public:
        Company() = default;
        ~Company();
        int get_earnings();
    
    private:
        void determine_business_cycle(float cycle_value);
        void generate_company_earnings();
}; 

#endif // COMPANY_H