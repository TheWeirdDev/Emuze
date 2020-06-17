#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "emulators/chip8/Chip8.h"
#include "emuze.h"
int main() {
    Emuze::startEmuze();
    return 0;
}
