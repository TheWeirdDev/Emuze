//
// Created by alireza on 5/30/20.
//

#include "Chip8.h"

namespace Emuze::Chip8 {

Chip8::Chip8Sound::Chip8Sound() {
    const double TWO_PI = 6.28318;
    const double increment = 440. / 44100;
    double x = 0;
    samples.reserve(SAMPLES);
    for (int i = 0; i < SAMPLES; i++) {
        samples[i] = AMPLITUDE * sin(x * TWO_PI);
        x += increment;
    }
    initSound();
}

void Chip8::Chip8Sound::initSound() {
    if (!buffer.loadFromSamples(&samples.front(), SAMPLES, 1, SAMPLE_RATE)) {
        spdlog::error("Loading failed!");
        throw std::exception();
    }
    sound.setBuffer(buffer);
    sound.setLoop(true);
}

Chip8::Chip8Sound::~Chip8Sound() { sound.stop(); }
}  // namespace Emuze::Chip8