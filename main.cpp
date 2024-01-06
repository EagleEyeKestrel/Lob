#include <chrono>
#include <iostream>
#include "commandUtils.cpp"
#include "mainWorker.cpp"



int main(int argc, const char *argv[]) {
    auto start = std::chrono::steady_clock::now();


    Config cfg = parseArgs(argc, argv);
    readRawInput(cfg.input, cfg.ifBinary);
    std::cout << "simulate starts\n";
    simulateStocks();
    std::cout << "sort starts\n";
    sortSplitSnaps();
    std::cout << "combine starts\n";
    combineSplitFiles(cfg.output);

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
    return 0;
}
