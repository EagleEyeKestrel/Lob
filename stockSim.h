//
// Created by ji luyang on 2023/12/30.
//

#ifndef LOB_STOCKSIM_H
#define LOB_STOCKSIM_H

#include <map>
#include <list>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include "order.h"
#include "otstruct.h"
#include "global.h"

class StockSimulator {
public:
    // ctor & dtor
//    StockSimulator();
    StockSimulator(std::string &stockFilePath);

    // tackle one orderTrade
    void solveOrderTrade(const OrderRaw &orderRaw);
    void solveAskBidOrder(const OrderRaw &orderRaw);
    void solveExecutedOrder(const OrderRaw &orderRaw);
    void solveCanceledOrder(const OrderRaw &orderRaw);

    //L5
    bool checkStable() const;
    void updL5();

    //print
    std::string formatLine() const;
    void saveSnap();

    void run();

private:
    FILE *infile;
    std::vector<FILE*> outfile;
    std::string filePath;
    std::string date;
    std::string symbol;
    std::set<int> bids;                                                  // prices
    std::set<int> asks;
    std::unordered_map<int, Order> bidMap;                                  // index -> order
    std::unordered_map<int, Order> askMap;
    std::unordered_map<int, long long> bidVolume;                           // price -> volume
    std::unordered_map<int, long long> askVolume;

    long long totalVolume;
    long long totalAmount;  // real amount * 100
    int lastExecutedPrice;
    int lastProcessedIndex;
    int lastProcessedSecond;


//    L5 l5Now;
    L5 l5Snap;

};

#endif //LOB_STOCKSIM_H
