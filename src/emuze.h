#pragma once

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <iostream>
#include <thread>
#include <vector>

#include "emulators/chip8/Chip8.h"
namespace Emuze {
static constexpr int SCREEN_WIDTH = 900, SCREEN_HEIGHT = 600;
int startEmuze();
}  // namespace Emuze
