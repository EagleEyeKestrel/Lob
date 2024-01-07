//
// Created by ji luyang on 2023/12/30.
//

#ifndef LOB_ORDER_H
#define LOB_ORDER_H
#include <cstring>
#include <vector>
#include <string>
#include "otstruct.h"

struct Order {
    Order() {}
    Order(const OrderRaw &raw) {
        orderID = raw.index;
        price = static_cast<int>(100 * raw.price);
        volume = raw.volume;
        direction = raw.direction;
        symbol = std::string(raw.securityID);
        date = raw.tradingDay;
        timestamp = raw.exchangeTime;
    }
    std::string symbol;
    int orderID;
    int price; // * 100的价格
    int volume;
    int date;
    int timestamp;
    char direction;  // 0:bidorder, 1:askorder
};

struct Layer {
    Layer(): price(-1), volume(0) {}
    Layer(int _price, int _volume): price(_price), volume(_volume) {}

    int price;
    int volume;
};

struct L5 {
    L5() {
        LAYERS = 5;
        bidL5 = std::vector<Layer>(LAYERS);
        askL5 = std::vector<Layer>(LAYERS);
    }
    L5(const L5 &l): bidL5(l.bidL5), askL5(l.askL5) {}
    std::vector<Layer> bidL5;
    std::vector<Layer> askL5;
    int LAYERS;
};

#endif //LOB_ORDER_H
