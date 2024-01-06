//
// Created by ji luyang on 2024/1/4.
//

#ifndef LOB_WORKER_H
#define LOB_WORKER_H

#include <stdio.h>
#include <unordered_map>
#include <filesystem>
#include "order.h"

namespace fs = std::filesystem;

class FileParser {
public:
    ~FileParser();
    void solveCSVRaw(const char *path);
    void solveBinaryRaw(const char *path);
    void saveStockOrderRaw(const OrderRaw &orderRaw);

private:
    FILE *infile;
    std::unordered_map<std::string, FILE*> symbolFile;
};


class FileSorter {
public:
    FileSorter(const std::string &path);
    int getSecondByLine(const std::string &line);
    void saveSortedRecords();
    void run();
//private:
    FILE *infile;
    FILE *outfile;
    std::string fileName;
    std::vector<std::pair<int, std::string>> records;
};

void mergeFiles(const char *path, const char *output);

#endif //LOB_WORKER_H
