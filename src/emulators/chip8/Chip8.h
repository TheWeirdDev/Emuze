#ifndef EMUZE_CHIP8_H
#define EMUZE_CHIP8_H

#include <spdlog/spdlog.h>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Config.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <algorithm>
#include <random>
#include <stack>
#include <string>
#include <vector>
#include <utility>

#include "../../components/EmulatorView.h"
#include "../../emuze.h"
#include "Disasm.h"

namespace Emuze::Chip8 {
using sf::Uint8, sf::Uint16, sf::Uint32;

constexpr std::size_t MEM_SIZE = 0x1000;

class Chip8 final : public EmulatorView {
   public:
    static constexpr auto DISPLAY_COLUMNS = 64, DISPLAY_ROWS = 32,
                          PIXEL_SCALE = 10.0, INSTRUCTION_LENGTH = 2;

   private:
    class Chip8Sound final {
       private:
        static constexpr auto SAMPLE_RATE = 44100, AMPLITUDE = 30000,
                              SAMPLES = 44100;
        void initSound(const sf::Int16* first);
        sf::Sound sound;
        sf::SoundBuffer buffer;

       public:
        Chip8Sound();
        ~Chip8Sound() { sound.stop(); };
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

    std::array<Uint8, MEM_SIZE> memory{};
    std::array<Uint8, 16> V{};

    std::array<std::array<bool, DISPLAY_COLUMNS>, DISPLAY_ROWS> video{};
    std::stack<Uint16> c8stack{};
    static constexpr Uint32 base = 0x200;
    Uint16 PC = base;
    Uint32 I = 0;
    static constexpr Uint8 UNSET_KEY = 0x10;
    Uint8 pressedKey = UNSET_KEY;
    Uint8 waitingX = UNSET_KEY;
    int DT = 0;
    int ST = 0;
    std::string currentRom;
    sf::Font forward_font;
    bool finished = true;
    bool debug = false;
    std::map<const int, std::string> disasm;


   public:
    explicit Chip8();
    Chip8(const Chip8&) = delete;
    Chip8(Chip8&&) = delete;
    Chip8& operator=(const Chip8&) = delete;
    Chip8& operator=(Chip8&&) = delete;
    void setPressedKey(sf::Keyboard::Key);
    void setReleasedKey();
    void reset();
    void openRom(const std::string&);
    void step();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void finish();
    bool isFinished() { return finished; }
    bool isDebug() { return debug; }
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
