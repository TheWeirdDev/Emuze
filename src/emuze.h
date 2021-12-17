#ifndef EMUZE_H
#define EMUZE_H

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <array>
#include <iostream>
#include <vector>

#include "emulators/chip8/Chip8.h"
namespace Emuze {
static constexpr int SCREEN_WIDTH = 1100, SCREEN_HEIGHT = 650,
                     STEPS_PER_FRAME = 16;
int startEmuze();
}  // namespace Emuze

#endif
