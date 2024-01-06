//
// Created by ji luyang on 2024/1/5.
//
#include <string>

struct Config {
    bool ifBinary;
    const char *input;
    const char *output;
};

Config parseArgs(int argc, const char *argv[]) {
    int inputKind = 0, f = 0, o = 0;
    Config cfg;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-c")) {
            inputKind++;
            cfg.ifBinary = false;
        }
        if (!strcmp(argv[i], "-b")) {
            inputKind++;
            cfg.ifBinary = true;
        }
        if (!strcmp(argv[i], "-f")) {
            cfg.input = argv[i + 1];
            ++f;
        }
        if (!strcmp(argv[i], "-o")) {
            cfg.output = argv[i + 1];
            ++o;
        }
    }
    if (!(inputKind == 1 && f == 1 && o == 1)) {
        printf("Invalid command line args.\n");
        exit(0);
    }
    return cfg;
}