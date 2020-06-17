//
// Created by alireza on 5/30/20.
//

#ifndef EMUZE_CHIP8_H
#define EMUZE_CHIP8_H

#include <spdlog/spdlog.h>

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Config.hpp>
#include <array>
#include <cmath>
#include <vector>

#include "Disasm.h"
#include "VM.h"

namespace Emuze::Chip8 {
class Chip8 {
   private:
    class Chip8Sound final {
       private:
        static constexpr unsigned SAMPLE_RATE = 44100;
        static constexpr unsigned AMPLITUDE = 30000;
        static constexpr unsigned SAMPLES = 44100;
        std::vector<sf::Int16> samples{};

        void initSound();
        sf::Sound sound;
        sf::SoundBuffer buffer;

       public:
        Chip8Sound();
        ~Chip8Sound();
        Chip8Sound(const Chip8Sound&) = delete;
        Chip8Sound(Chip8Sound&&) = delete;
        Chip8Sound& operator=(const Chip8Sound&) = delete;
        Chip8Sound& operator=(Chip8Sound&&) = delete;

        inline void play() { sound.play(); }
        inline void stop() { sound.stop(); }
    };
    Chip8Sound sound{};
    VM vm;

   public:
};
}  // namespace Emuze::Chip8
#endif  // EMUZE_CHIP8_H
