//
// Created by ji luyang on 2023/12/30.
//

#include "stockSim.h"
#include <string>
#include <filesystem>
#include <stdio.h>
#include <thread>
#include "dateUtils.cpp"

namespace fs = std::filesystem;


StockSimulator::StockSimulator(std::string &stockFilePath) {
    totalAmount = 0;
    totalVolume = 0;
    lastExecutedPrice = -1;
    lastProcessedIndex = -1;
    lastProcessedSecond = -1;
    fs::path p(stockFilePath);
    std::string stockFileName = p.filename().string();

    int dash = stockFileName.find('-');
    date = stockFileName.substr(dash + 1, stockFileName.size() - dash - 5);
    symbol = stockFileName.substr(0, dash);
    filePath = stockFilePath;

//    std::cout << "date: " << date << " symbol: " << symbol << "\n";
//    std::cout << "open split file: " << stockFilePath << "\n";

}


void StockSimulator::solveOrderTrade(const OrderRaw &orderRaw) {
//    std::cerr << "solve order trade type: " << (int)orderRaw.fieldType << "\n";

    if (orderRaw.fieldType == 0) {
        solveAskBidOrder(orderRaw);
    } else if (orderRaw.fieldType == 1) {
        solveExecutedOrder(orderRaw);
    } else {
        solveCanceledOrder(orderRaw);
    }
    lastProcessedIndex = orderRaw.index;
//    std::cerr << "In symbol: " << orderRaw.securityID << " last index changes to " << orderRaw.index << "\n";
    lastProcessedSecond = getSecond(orderRaw.exchangeTime);
}

void addOrderInMem(const Order &order, std::set<int> &priceSet, std::unordered_map<int, Order> &orderMap,
                   std::unordered_map<int, long long> &volumeMap) {

    if (volumeMap[order.price] == 0) {
        priceSet.insert(order.price);
    }
    orderMap[order.orderID] = order;
    volumeMap[order.price] += order.volume;
}

void StockSimulator::solveAskBidOrder(const OrderRaw &orderRaw) {
    Order order(orderRaw);
    if (order.direction == 0) {
        addOrderInMem(order, bids, bidMap, bidVolume);
    } else {
        addOrderInMem(order, asks, askMap, askVolume);
    }

}

void decOrderInMem(int orderID, int volume, std::set<int> &priceSet, std::unordered_map<int, Order> &orderMap,
                   std::unordered_map<int, long long> &volumeMap) {
    int originPrice = orderMap[orderID].price;
    orderMap[orderID].volume -= volume;
    volumeMap[originPrice] -= volume;
    if (volumeMap[originPrice] == 0) {
        priceSet.erase(originPrice);
    }
}

void StockSimulator::solveExecutedOrder(const OrderRaw &orderRaw) {
    decOrderInMem(orderRaw.bidIndex, orderRaw.volume, bids, bidMap, bidVolume);
    decOrderInMem(orderRaw.askIndex, orderRaw.volume, asks, askMap, askVolume);

    totalVolume += orderRaw.volume;
    int price = (int)(100 * orderRaw.price);
    totalAmount += (long long)orderRaw.volume * price;
    lastExecutedPrice = price;
}

void StockSimulator::solveCanceledOrder(const OrderRaw &orderRaw) {
    if (orderRaw.bidIndex) {
        decOrderInMem(orderRaw.bidIndex, orderRaw.volume, bids, bidMap, bidVolume);
    } else {
        decOrderInMem(orderRaw.askIndex, orderRaw.volume, asks, askMap, askVolume);
    }
}

void StockSimulator::updL5() {
    l5Snap = L5();
    std::set<int>::iterator iter = asks.begin();
    for (int i = 0; i < std::min(l5Snap.LAYERS, (int)asks.size()); ++i) {
        int layerPrice = *iter;
        l5Snap.askL5[i] = Layer(layerPrice, askVolume[layerPrice]);
        ++iter;
    }
    std::set<int>::reverse_iterator riter = bids.rbegin();
    for (int i = 0; i < std::min(l5Snap.LAYERS, (int)bids.size()); ++i) {
        int layerPrice = *riter;
        l5Snap.bidL5[i] = Layer(layerPrice, bidVolume[layerPrice]);
        ++riter;
    }
}

std::string StockSimulator::formatLine() const {
    std::string res = std::to_string(lastProcessedIndex);
    res.push_back('|');
    res += symbol;
    res.push_back('|');
    res += date;
    res.push_back('|');
    std::string timestamp = intToStr(lastProcessedSecond, 6);
    res += timestamp;
    res.push_back('|');
    res += std::to_string(totalVolume);
    res.push_back('|');
    res += realPrice(totalAmount);
    res.push_back('|');
    res += realPrice(lastExecutedPrice);
    res.push_back('|');
    for (int i = l5Snap.LAYERS - 1; i >= 0; --i) {
        res += realPrice(l5Snap.bidL5[i].price);
        res.push_back('|');
        res += std::to_string(l5Snap.bidL5[i].volume);
        res.push_back('|');
    }
    for (int i = 0; i < l5Snap.LAYERS; ++i) {
        res += realPrice(l5Snap.askL5[i].price);
        res.push_back('|');
        res += std::to_string(l5Snap.askL5[i].volume);
        if (i != l5Snap.LAYERS - 1) {
            res.push_back('|');
        }
    }
    return res;
}

bool StockSimulator::checkStable() const {
    if (!bids.empty() && !asks.empty() && *bids.rbegin() >= *asks.begin())
        return false;
    return true;
}

void StockSimulator::saveSnap() {
    std::string line = formatLine();

    int ofID = getOutFileID(lastProcessedSecond * 1000);
    int sz = line.size();

    std::lock_guard<std::mutex> lock(mutexes[ofID]);
    fseek(outfile[ofID], 0, SEEK_END);
    fwrite(&sz, sizeof(int), 1, outfile[ofID]);
    const char *lineStr = line.c_str();
    fwrite(lineStr, sizeof(char), sz, outfile[ofID]);
}


void StockSimulator::run() {
    std::cout << filePath << " begins simulating\n";
    infile = std::fopen(filePath.c_str(), "rb");
    outfile = std::vector<FILE*>(TimeBlocks, nullptr);
    for (int ofID = 0; ofID < TimeBlocks; ofID++) {
        std::string ofPrefix = "./snapdata/";
        std::string ofSuffix = ".dat";
        std::string ofName = ofPrefix + intToStr(ofID, 3) + ofSuffix;
        fs::create_directories(ofPrefix);

        std::lock_guard<std::mutex> lock(mutexes[ofID]);
        outfile[ofID] = std::fopen(ofName.c_str(), "ab+");
    }


    OrderRaw orderRaw;
    while (fread(&orderRaw, sizeof(orderRaw), 1, infile) == 1) {
//        std::cerr << "solving order index: " << orderRaw.index << ' ' << orderRaw.exchangeTime << "\n";
        int nowSecond = getSecond(orderRaw.exchangeTime);
        if (lastProcessedSecond >= 0 && nowSecond > lastProcessedSecond) {
            saveSnap();
        }
        solveOrderTrade(orderRaw);
//        std::cerr << "finish solving order index: " << orderRaw.index << ' ' << orderRaw.exchangeTime << "\n";
        if (checkStable()) {
            updL5();
        }

    }
    saveSnap();


    if (infile != nullptr) {
        std::fclose(infile);
    }
    for (int i = 0; i < TimeBlocks; i++) {
//        std::cout << "In run last: " << filePath << ", acquire lock\n";
        std::lock_guard<std::mutex> lock(mutexes[i]);
        if (outfile[i] != nullptr) {
            std::fclose(outfile[i]);
        }
    }
}