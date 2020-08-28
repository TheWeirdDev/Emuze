#include <iostream>

#include "emuze.h"
int main([[maybe_unused]] int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: ./Emuze RomFile\n";
        return 0;
    }
    // std::cout << argv[0] << '\n';
    Emuze::startEmuze(argv[1]);
    return 0;
}
