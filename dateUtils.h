//
// Created by ji luyang on 2024/1/7.
//

#ifndef LOB_DATEUTILS_H
#define LOB_DATEUTILS_H
#include <string>

int getSecond(int timestamp);
int getOutFileID(int timestamp);
std::string intToStr(int integer, int digits);
std::string realPrice(long long price);

#endif //LOB_DATEUTILS_H
