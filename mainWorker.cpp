//
// Created by ji luyang on 2024/1/5.
//

#include "threadPool.h"
#include "stockSim.h"
#include "fileWorker.h"
#include <filesystem>
#include <thread>
#include "dateUtils.h"
#include <iostream>

namespace fs = std::filesystem;
std::vector<FILE*> StockSimulator::outfile = std::vector<FILE*>(TimeBlocks, nullptr);

void readRawInput(const char *input, bool ifBinary) {
    FileParser fp;
    if (ifBinary) {
        fp.solveBinaryRaw(input);
    } else {
        fp.solveCSVRaw(input);
    }
}


void simulateStocks() {
    ThreadPool pool(8);
    pool.init();

    for (int ofID = 0; ofID < TimeBlocks; ofID++) {
        std::string ofPrefix = "./snapdata/";
        std::string ofSuffix = ".dat";
        std::string ofName = ofPrefix + intToStr(ofID, 3) + ofSuffix;
        fs::create_directories(ofPrefix);

        std::lock_guard<std::mutex> lock(mutexes[ofID]);
        StockSimulator::outfile[ofID] = fopen(ofName.c_str(), "ab+");
    }

    auto ithSimulator = [](StockSimulator &sim) {
        sim.run();
    };

    fs::path dir = "./input_split_data";
    std::vector<std::future<void>> futures;
    for (auto &entry: fs::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            std::string fileName = entry.path().filename().string();
            if (fileName.size() > 4 && fileName.substr(fileName.size() - 4) == ".dat" && fileName.find('-') != std::string::npos) {
                std::string stockFile = dir.string() + "/" + fileName;
                StockSimulator sim(stockFile);
                futures.emplace_back(pool.submit(ithSimulator, sim));
//                futures.emplace_back(pool.submit(std::bind(&StockSimulator::run, &(sims.back()))));
//                futures.emplace_back(pool.submit(std::mem_fn(&StockSimulator::run), &(sims.back())));
            }
        }
    }

//    std::cout << "outfile initialized\n";

    for (auto &fu: futures) {
        fu.get();
    }
    pool.shutdown();
    for (int ofID = 0; ofID < TimeBlocks; ofID++) {
        fclose(StockSimulator::outfile[ofID]);
    }
}

void sortSplitSnaps() {
    ThreadPool pool(8);
    pool.init();

    fs::path dir = "./snapdata";
    std::vector<std::future<void>> futures;

    auto ithSorter = [](FileSorter &sorter) {
        sorter.run();
    };

    for (auto &entry: fs::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            std::string fileName = entry.path().filename().string();
            if (fileName.size() > 4 && fileName.substr(fileName.size() - 4) == ".dat") {
                std::string snapFile = dir.string() + "/" + fileName;
                futures.emplace_back(pool.submit(ithSorter, FileSorter(snapFile)));
            }
        }
    }


    for (auto &fu: futures) {
        fu.get();
    }
    pool.shutdown();
}



void combineSplitFiles(const char *output) {
    mergeFiles("./snapdata", output);
}