#include <filesystem>
#include <iostream>

#include "emuze.h"
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: ./Emuze RomFile\n";
        return 0;
    }
    if (!std::filesystem::exists(argv[1])) {
        spdlog::error("This rom file doesn't exist");
        return 1;
    }
    Emuze::startEmuze(argv[1]);
    return 0;
}
