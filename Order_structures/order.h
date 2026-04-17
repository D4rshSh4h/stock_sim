#ifndef ORDER_H
#define ORDER_H

class Order {
private:
    float price;
    int id; //ID of agent which placed the order
    int trade_type; // 0 for buy, 1 for sell
    int time_placed;
    int state; //0 for active, 1 for timeouted

public:
    Order(int price, int id, int trade_type, int time_placed, int state);
    Order() = default;

    float getPrice() const;
    int getId() const;
    int getTradeType() const;
    int getTimePlaced() const;
    int getStatus() const;

    void timeout_order();
};

#endif // ORDER_H