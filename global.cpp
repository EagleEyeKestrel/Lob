//
// Created by ji luyang on 2024/1/6.
//

#include "global.h"

int TimeBlocks = 240;
std::vector<std::mutex> mutexes = std::vector<std::mutex>(TimeBlocks);