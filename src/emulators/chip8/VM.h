#ifndef CHIP8_VM_H
#define CHIP8_VM_H

#include <SFML/System.hpp>
#include <array>

constexpr std::size_t MemSize = 0x1000;

namespace Emuze::Chip8 {
class VM {
    std::array<sf::Uint8, MemSize> memory{};
    std::array<sf::Uint8, 16> V{};
    std::array<sf::Uint8, 0xFF> video{};
};
}  // namespace Emuze::Chip8
#endif