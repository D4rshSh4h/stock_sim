#ifndef ORDER_H
#define ORDER_H

class Order {
private:
    int price;
    int id;

public:
    Order(int price, int id);

    int getPrice() const;
    int getId() const;
};

#endif // ORDER_H