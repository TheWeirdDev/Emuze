#include <iostream>

#include "emuze.h"
int main([[maybe_unused]] int argc, char** argv) {
    std::cout << argv[0] << '\n';
    Emuze::startEmuze();
    return 0;
}
