//
// Created by ji luyang on 2024/1/3.
//
#include "dateUtils.h"

int getSecond(int timestamp) {
    return timestamp / 1000;
}

int getOutFileID(int timestamp) {
    // HHMMSSsss -> HHM
    return timestamp / 1000000;
}

std::string intToStr(int integer, int digits) {
    std::string res = std::to_string(integer);
    while (res.size() < digits) {
        res.insert(res.begin(), '0');
    }
    return res;
}

std::string realPrice(long long price) {
    // convert 1234 -> 12.34
    if (price < 0)
        return "nan";
    if (price == 0)
        return "0.00";
    if (price < 100) {
        return "0." + std::to_string(price);
    }
    std::string s = std::to_string(price);
    return s.substr(0, s.size() - 2) + "." + s.substr(s.size() - 2);
}