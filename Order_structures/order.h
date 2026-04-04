#ifndef ORDER_H
#define ORDER_H

class Order {
private:
    float price;
    int id; //ID of agent which placed the order
    int trade_type; // 0 for buy, 1 for sell

public:
    Order(int price, int id, int trade_type);
    Order() = default;

    float getPrice() const;
    int getId() const;
    int getTradeType() const;
};

#endif // ORDER_H