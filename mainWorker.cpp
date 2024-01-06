//
// Created by ji luyang on 2024/1/5.
//

#include "threadPool.h"
#include "stockSim.h"
#include "fileWorker.h"
#include <filesystem>
#include <thread>

namespace fs = std::filesystem;


void readRawInput(const char *input, bool ifBinary) {
    FileParser fp;
    if (ifBinary) {
        fp.solveBinaryRaw(input);
    } else {
        fp.solveCSVRaw(input);
    }
}

void simulateStocks() {
//    ThreadPool pool(8);
//    pool.init();
    const int NUMTHREADS = 8;
    std::thread threads[NUMTHREADS];


    fs::path dir = "./input_split_data";
//    std::vector<std::future<void>> futures;
    std::vector<StockSimulator> sims;
    for (auto &entry: fs::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            std::string fileName = entry.path().filename().string();
            if (fileName.size() > 4 && fileName.substr(fileName.size() - 4) == ".dat" && fileName.find('-') != std::string::npos) {
                std::string stockFile = dir.string() + "/" + fileName;
                StockSimulator sim(stockFile);
                sims.push_back(sim);
//                futures.emplace_back(pool.submit(std::bind(&StockSimulator::run, &(sims.back()))));
//                futures.emplace_back(pool.submit(std::mem_fn(&StockSimulator::run), &(sims.back())));
            }
        }
    }
    auto ithSimulator = [&](int id) {
        for (int symbol = id; symbol < sims.size(); symbol += NUMTHREADS) {
            sims[symbol].run();
        }
    };
    for (int i = 0; i < NUMTHREADS; i++) {
        threads[i] = std::thread(ithSimulator, i);
    }
    for (int i = 0; i < NUMTHREADS; i++) {
        threads[i].join();
    }
//    for (auto &fu: futures) {
//        fu.get();
//    }
//    pool.shutdown();
}

void sortSplitSnaps() {
//    ThreadPool pool(8);
//    pool.init();
    const int NUMTHREADS = 8;
    std::thread threads[NUMTHREADS];
    fs::path dir = "./snapdata";
//    std::vector<std::future<void>> futures;
    std::vector<FileSorter> sorters;

    for (auto &entry: fs::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            std::string fileName = entry.path().filename().string();
            if (fileName.size() > 4 && fileName.substr(fileName.size() - 4) == ".dat") {
                std::string snapFile = dir.string() + "/" + fileName;
//                std::cerr << "snap file: " << snapFile <<  "\n";
                sorters.emplace_back(snapFile);
//                sorters.back().run();
//                futures.emplace_back(pool.submit(std::bind(&FileSorter::run, &(sorters.back()))));
//                futures.emplace_back(pool.submit(std::mem_fn(&FileSorter::run), &(sorters.back())));
            }
        }
    }

    auto ithSorter = [&](int id) {
        for (int i = id; i < sorters.size(); i += NUMTHREADS) {
            sorters[i].run();
        }
    };
    for (int i = 0; i < NUMTHREADS; i++) {
        threads[i] = std::thread(ithSorter, i);
    }
    for (int i = 0; i < NUMTHREADS; i++) {
        threads[i].join();
    }
//    for (auto &fu: futures) {
//        fu.get();
//    }
//    pool.shutdown();
}



void combineSplitFiles(const char *output) {
    mergeFiles("./snapdata", output);
}