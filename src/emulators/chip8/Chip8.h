//
// Created by alireza on 5/30/20.
//

#ifndef EMUZE_CHIP8_H
#define EMUZE_CHIP8_H

#include <spdlog/spdlog.h>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Config.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <cmath>
#include <fstream>
#include <random>
#include <stack>
#include <string>
#include <thread>
#include <vector>

#include "../../components/EmulatorView.h"
#include "Disasm.h"

namespace Emuze::Chip8 {
using sf::Uint8, sf::Uint16, sf::Uint32;

constexpr std::size_t MemSize = 0x1000;

class Chip8 final : public EmulatorView {
   private:
    class Chip8Sound final {
       private:
        static constexpr unsigned SAMPLE_RATE = 44100;
        static constexpr unsigned AMPLITUDE = 30000;
        static constexpr unsigned SAMPLES = 44100;

        void initSound(const sf::Int16* first);
        sf::Sound sound;
        sf::SoundBuffer buffer;

       public:
        Chip8Sound();
        ~Chip8Sound();
        Chip8Sound(const Chip8Sound&) = delete;
        Chip8Sound(Chip8Sound&&) = delete;
        Chip8Sound& operator=(const Chip8Sound&) = delete;
        Chip8Sound& operator=(Chip8Sound&&) = delete;

        inline void play() {
            if (sound.getStatus() != sf::Sound::Playing) sound.play();
        }
        inline void stop() { sound.stop(); }
    };
    Chip8Sound sound{};

    std::array<Uint8, MemSize> memory{};
    std::array<Uint8, 16> V{};
    std::array<std::array<bool, 64>, 32> video{};
    std::stack<Uint16> c8stack{};
    Uint32 SP = 0;
    Uint32 base = 0x200;
    Uint16 PC = base;
    std::size_t romSize = 0;
    Uint32 I = 0;
    Uint8 pressedKey = 0x10;
    Uint8 waitingX = 0x10;
    std::size_t DT = 0;
    std::size_t ST = 0;
    std::size_t clock = 0;
    std::jthread timer_thread;

   public:
    explicit Chip8();
    void setPressedKey(sf::Keyboard::Key);
    void setReleasedKey();
    void reset();
    void openRom(const std::string&);
    void step();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void finish();
};

static const constinit std::array<const std::array<Uint8, 5>, 16> digits{{
    {0b11110000, 0b10010000, 0b10010000, 0b10010000, 0b11110000},
    {0b00100000, 0b01100000, 0b00100000, 0b00100000, 0b01110000},
    {0b11110000, 0b00010000, 0b11110000, 0b10000000, 0b11110000},
    {0b11110000, 0b00010000, 0b11110000, 0b00010000, 0b11110000},
    {0b10010000, 0b10010000, 0b11110000, 0b00010000, 0b00010000},
    {0b11110000, 0b10000000, 0b11110000, 0b00010000, 0b11110000},
    {0b11110000, 0b10000000, 0b11110000, 0b10010000, 0b11110000},
    {0b11110000, 0b00010000, 0b00100000, 0b01000000, 0b01000000},
    {0b11110000, 0b10010000, 0b11110000, 0b10010000, 0b11110000},
    {0b11110000, 0b10010000, 0b11110000, 0b00010000, 0b11110000},
    {0b11110000, 0b10010000, 0b11110000, 0b10010000, 0b10010000},
    {0b11100000, 0b10010000, 0b11100000, 0b10010000, 0b11100000},
    {0b11110000, 0b10000000, 0b10000000, 0b10000000, 0b11110000},
    {0b11100000, 0b10010000, 0b10010000, 0b10010000, 0b11100000},
    {0b11110000, 0b10000000, 0b11110000, 0b10000000, 0b11110000},
    {0b11110000, 0b10000000, 0b11110000, 0b10000000, 0b10000000},
}};

}  // namespace Emuze::Chip8
#endif  // EMUZE_CHIP8_H
