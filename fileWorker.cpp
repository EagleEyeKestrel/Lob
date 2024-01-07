//
// Created by ji luyang on 2024/1/4.
//

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include "fileWorker.h"


int nextIntInCSV(const std::string &str, int &l) {
    int r = l, res = str[l] - '0';
    while (r + 1 < str.size() && isdigit(str[r + 1])) {
        ++r;
        res = res * 10 + str[r] - '0';
    }
    l = r + 2;
    return res;
}

OrderRaw csvLineToOrderRaw(const std::string &str) {
    OrderRaw res;
    int l = 0;
    res.fieldType = nextIntInCSV(str, l);
    res.index = nextIntInCSV(str, l);
    int r = l;
    while (str[r] != '|') {
        res.securityID[r - l] = str[r];
        ++r;
    }
    res.securityID[r - l] = '\0';
    l = r + 1;
    res.tradingDay = nextIntInCSV(str, l);
    res.exchangeTime = nextIntInCSV(str, l);
    r = l;
    while (str[r] != '|')
        ++r;
    res.price = stod(str.substr(l, r - l));
    l = r + 1;
    res.volume = nextIntInCSV(str, l);
    res.direction = nextIntInCSV(str, l);
    res.askIndex = nextIntInCSV(str, l);
    res.bidIndex = nextIntInCSV(str, l);
    return res;
}

FileParser::~FileParser() {
    for (auto [key, p]: symbolFile) {
        fclose(p);
    }
}

void FileParser::saveStockOrderRaw(const OrderRaw &orderRaw) {
    const char *security = orderRaw.securityID;
    int today = orderRaw.tradingDay;
    std::string symbol(security);
    if (!symbolFile.count(symbol)) {
        std::string prefix = "./input_split_data/";
        std::string suffix = ".dat";
        std::string todayStr = std::to_string(today);
        std::string fileName = prefix + symbol + "-" + todayStr + suffix;

        fs::create_directories(prefix);
        symbolFile[symbol] = fopen(fileName.c_str(), "wb+");
    }
    FILE *stockRaw = symbolFile[symbol];
    fwrite(&orderRaw, sizeof(orderRaw), 1, stockRaw);
}

void FileParser::solveCSVRaw(const char *path) {
    infile = fopen(path, "r");
    if (infile == nullptr) {
        printf("Error while opening CSV file: %s", path);
        exit(1);
    }
    char line[1000];
    fgets(line, 1000, infile); // header
    while (fgets(line, 1000, infile)) {
        if (strlen(line) < 5) {
            break;
        }
        std::string str(line);
        OrderRaw orderRaw = csvLineToOrderRaw(str);
        saveStockOrderRaw(orderRaw);
    }
    fclose(infile);
}

void FileParser::solveBinaryRaw(const char *path) {
    infile = fopen(path, "rb");
    if (infile == nullptr) {
        printf("Error while opening Binary file: %s", path);
        exit(1);
    }
    OrderRaw orderRaw;
    std::cout << "read raw begin\n";
    while (fread(&orderRaw, sizeof(orderRaw), 1, infile) == 1) {
        saveStockOrderRaw(orderRaw);
    }
    fclose(infile);
    std::cout << "read raw done\n";
}


FileSorter::FileSorter(const std::string &path) {
    fileName = path;
}

int FileSorter::getSecondByLine(const std::string &line) {
    int thirdOr = -1, fourthOr = -1, cnt = 0;
    for (int i = 0; i < line.size(); ++i) {
        if (line[i] == '|') {
            ++cnt;
            if (cnt == 3) {
                thirdOr = i;
            }
            if (cnt == 4) {
                fourthOr = i;
                break;
            }
        }
    }
    if (fourthOr == -1) {
        printf("Error occurred in file: %s, line: %s", fileName.c_str(), line.c_str());
        exit(1);
    }
    return stoi(line.substr(thirdOr + 1, fourthOr - thirdOr - 1));
}

void FileSorter::saveSortedRecords() {
    outfile = fopen(fileName.c_str(), "w");
    fclose(outfile);
    outfile = fopen(fileName.c_str(), "ab+");
    for (std::pair<int, std::string> &record: records) {
        std::string &line = record.second;
        const char *p = line.c_str();
        int sz = line.size();
        fwrite(&sz, sizeof(int), 1, outfile);
        fwrite(p, sz, 1, outfile);
    }
    fclose(outfile);
}

void FileSorter::run() {
    infile = fopen(fileName.c_str(), "rb+");
    if (infile == nullptr) {
        printf("Error while sorting snap file %s", fileName.c_str());
        exit(1);
    }
    char line[1000] = {0};
    int sz;
    while (fread(&sz, sizeof(int), 1, infile) == 1) {
        fread(line, sizeof(char), sz, infile);
        line[sz] = '\0';
        std::string str(line);
        records.emplace_back(getSecondByLine(str), str);
    }


    fclose(infile);
    std::sort(records.begin(), records.end());
    saveSortedRecords();
}


void mergeFiles(const char *path, const char *output) {
    fs::path dir = path;
    std::vector<std::string> fileNames;
    for (auto &entry: fs::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            fileNames.push_back(entry.path().filename().string());
        }
    }
    std::sort(fileNames.begin(), fileNames.end());

    fs::path outdir = fs::path(output).parent_path();

    if (outdir.string().size() > 0 && !fs::exists(outdir)) {
        fs::create_directories(outdir);
    }

    FILE *outfile = fopen(output, "w");
    if (outfile == nullptr) {
        printf("Invalid output file %s", output);
        exit(1);
    }
    const char *header = "otindex|securityID|tradingDay|timeStamp|totalvolume|totalvalue|lastprice|bp5|bv5|bp4|bv4|bp3|bv3|bp2|bv2|bp1|bv1|ap1|av1|ap2|av2|ap3|av3|ap4|av4|ap5|av5\n";
    fwrite(header, strlen(header), 1, outfile);
    for (std::string &inputfile: fileNames) {
//        std::cout << inputfile << std::endl;
        std::string inputPath = std::string(path) + "/" + inputfile;
        FILE *infile = fopen(inputPath.c_str(), "rb");
        char line[1000] = {0};
        int sz;
        while (fread(&sz, sizeof(int), 1, infile) == 1) {
            fread(line, sz, 1, infile);
//            std::cout << sz << std::endl;
//            std::cout << line << std::endl;
            line[sz] = '\n';
            fwrite(line, sz + 1, 1, outfile);
        }
        fclose(infile);
    }
    fclose(outfile);
}