//
// Created by alireza on 5/30/20.
//

#include "Chip8.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <map>

namespace Emuze::Chip8 {

Chip8::Chip8()
    : timer_thread([&](const std::stop_token& t) {
          while (!t.stop_requested()) {
              bool needSleep = false;
              if (DT > 0) {
                  needSleep = true;
                  DT--;
              }
              if (ST > 0) {
                  sound.play();
                  needSleep = true;
                  ST--;
                  if (ST == 0) sound.stop();
              }
              if (needSleep) {
                  using namespace std::chrono_literals;
                  std::this_thread::sleep_for(17ms);
              }
          }
      }) {
    int index = 0;
    for (const auto& i : digits) {
        for (const auto& j : i) {
            memory[index++] = j;
        }
    }
}

Chip8::Chip8Sound::Chip8Sound() {
    std::vector<sf::Int16> samples{};
    const double TWO_PI = 6.28318;
    const double increment = 440. / 44100;
    double x = 0;
    samples.reserve(SAMPLES);
    for (size_t i = 0; i < SAMPLES; i++) {
        samples[i] = AMPLITUDE * sin(x * TWO_PI);
        x += increment;
    }
    initSound(&samples.front());
}

void Chip8::Chip8Sound::initSound(const sf::Int16* first) {
    if (!buffer.loadFromSamples(first, SAMPLES, 1, SAMPLE_RATE)) {
        spdlog::error("Loading failed!");
        throw std::exception();
    }
    sound.setBuffer(buffer);
    sound.setLoop(true);
}

Chip8::Chip8Sound::~Chip8Sound() { sound.stop(); }

void Chip8::openRom(const std::string& file) {
    reset();
    std::ifstream in_file(file, std::ios::in | std::ios::binary);
    // Disasm disasm;

    // Stop eating new lines in binary mode!!!
    in_file.unsetf(std::ios::skipws);

    in_file.seekg(0, std::ios::end);
    std::size_t fileSize = in_file.tellg();
    if (fileSize >= MemSize - 0x200) {
        std::cerr << "ERROR: File is too big\n";
        return;
    }
    in_file.seekg(0, std::ios::beg);
    if (!in_file.read((char*)(memory.data() + 0x200), fileSize)) {
        std::cerr << "Error while reading the rom file\n";
    }
    romSize = fileSize;
    for (unsigned int i = 0; i < fileSize; i += 2) {
        const auto addr = i + 0x200;
        std::cout << Disasm::disassemble(
                         sf::Uint16(memory[addr]) << 8u | memory[addr + 1],
                         addr)
                  << '\n';
    }
}
void Chip8::reset() {
    // TODO: reset memory
    // memory.fill(0);
    V.fill(0);
    for (auto& i : video) i.fill(false);
    std::stack<Uint16>().swap(c8stack);

    waitingX = 0x10;
    romSize = 0;
    SP = I = 0;
    PC = base;
    DT = ST = clock = 0;
}

void Chip8::setPressedKey(sf::Keyboard::Key keyCode) {
    static const std::map<sf::Keyboard::Key, Uint8> keys{
        {sf::Keyboard::X, 0},     {sf::Keyboard::Num1, 1},
        {sf::Keyboard::Num2, 2},  {sf::Keyboard::Num3, 3},
        {sf::Keyboard::Q, 4},     {sf::Keyboard::W, 5},
        {sf::Keyboard::E, 6},     {sf::Keyboard::A, 7},
        {sf::Keyboard::S, 8},     {sf::Keyboard::D, 9},
        {sf::Keyboard::Z, 10},    {sf::Keyboard::C, 11},
        {sf::Keyboard::Num4, 12}, {sf::Keyboard::R, 13},
        {sf::Keyboard::F, 14},    {sf::Keyboard::V, 15}};
    if (keys.contains(keyCode)) {
        if (waitingX != 0x10) {
            V[waitingX] = keys.at(keyCode);
            waitingX = 0x10;
        }
        pressedKey = keys.at(keyCode);
    }
}

void Chip8::step() {
    if (waitingX != 0x10) return;

    const unsigned short inst = Uint16(memory[PC]) << 8u | memory[PC + 1];
    // 12-bit literal Address
    const Uint16 a = inst & 0xfffU;

    // byte and nibble literals
    const Uint8 b = inst & 0xffU;
    const Uint8 n = inst & 0xfU;

    // vx and vy registers
    const Uint8 x = Uint16(inst >> 8U) & 0xfU;
    const Uint8 y = Uint16(inst >> 4U) & 0xfU;
    // spdlog::info(DT);
    // std::return a << b << n << x << y << '\n';
    if (inst == CLS) {
        for (auto& i : video) i.fill(false);
    }
    if (inst == RET) {
        PC = c8stack.top();
        c8stack.pop();
        SP--;
    }
    /*
    if (inst == LOW) {
        return fmt::format("{:04x} - LOW", i);
    }
    if (inst == HIGH) {
        return fmt::format("{:04x} - HIGH", i);
    }
    if (inst == SCR) {
        return fmt::format("{:04x} - SCR", i);
    }
    if (inst == SCL) {
        return fmt::format("{:04x} - SCL", i);
    }
    if (inst == EXIT) {
        return fmt::format("{:04x} - EXIT", i);
    }
    if ((inst & 0xFFF0) == 0x00B0) {
        return fmt::format("{:04x} - SCU    {}", i, n);
    }
    if (((inst & 0xFFF0)) == 0x00C0) {
        return fmt::format("{:04x} - SCD    {}", i, n);
    }*/
    if ((inst & INST_F000) == SYS) {
        // return fmt::format("{:04x} - SYS    #{}", i, a);
    }
    if ((inst & INST_F000) == JP_X) {
        PC = a;
        return;
    }
    if ((inst & INST_F000) == CALL) {
        SP++;
        c8stack.push(PC);
        PC = a;
        return;
    }
    if ((inst & INST_F000) == SE_VX) {
        if (V[x] == b) {
            PC += 2;
        }
    }
    if ((inst & INST_F000) == SNE_VX) {
        if (V[x] != b) {
            PC += 2;
        }
    }
    if ((inst & INST_F00F) == SE_VV) {
        if (V[x] == V[y]) {
            PC += 2;
        }
    }
    // if ((inst & INST_F00F) == SGT) {
    //     return fmt::format("{:04x} - SGT    V{}, V{}", i, x, y);
    // }
    // if ((inst & INST_F00F) == SLT) {
    //     return fmt::format("{:04x} - SLT    V{}, V{}", i, x, y);
    // }
    if ((inst & INST_F000) == LD_VX) {
        V[x] = b;
    }
    if ((inst & INST_F000) == ADD_VX) {
        V[x] += b;
    }
    if ((inst & INST_F00F) == LD_VV) {
        V[x] = V[y];
    }
    if ((inst & INST_F00F) == OR) {
        V[x] = V[x] | V[y];
    }
    if ((inst & INST_F00F) == AND) {
        V[x] = V[x] & V[y];
    }
    if ((inst & INST_F00F) == XOR) {
        V[x] = V[x] ^ V[y];
    }
    if ((inst & INST_F00F) == ADD_VV) {
        const Uint16 result = Uint16(V[x]) + Uint16(V[y]);
        V[0xF] = result >= 0xFFU;
        V[x] = result & 0xFFU;
    }
    if ((inst & INST_F00F) == SUB) {
        V[0xF] = V[x] > V[y];
        V[x] -= V[y];
    }
    if ((inst & INST_F00F) == SHR) {
        V[0xF] = V[x] & 0x1u;
        V[x] >>= 1u;
    }
    if ((inst & INST_F00F) == SUBN) {
        V[0xF] = V[y] > V[x];
        V[x] = V[y] - V[x];
    }
    if ((inst & INST_F00F) == SHL) {
        V[0xF] = V[x] & 0x8u;
        V[x] <<= 1u;
    }
    if ((inst & INST_F00F) == SNE_VV) {
        if (V[x] != V[y]) {
            PC += 2;
        }
    }
    // if ((inst & INST_F00F) == MUL) {
    //     return fmt::format("{:04x} - MUL    V{}, V{}", i, x, y);
    // }
    // if ((inst & INST_F00F) == DIV) {
    //     return fmt::format("{:04x} - DIV    V{}, V{}", i, x, y);
    // }
    // if ((inst & INST_F00F) == BCD_VV) {
    //     return fmt::format("{:04x} - BCD    V{}, V{}", i, x, y);
    // }
    if ((inst & INST_F000) == LD_IX) {
        I = a;
    }
    if ((inst & INST_F000) == JP_V0X) {
        PC = V[0] + a;
        return;
    }
    if ((inst & INST_F000) == RND) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution dist255(1, 255);
        V[x] = dist255(rng) & b;
    }
    if ((inst & INST_F000) == DRW) {
        V[0xF] = 0;
        for (int line = 0; line < n; line++) {
            const auto data = memory[I + line];
            auto currentPixel = 7u;
            for (int pixel = 0; pixel < 8; pixel++, currentPixel--) {
                const auto mask = 1u << currentPixel;
                if (data & mask) {
                    const auto w = (V[x] + pixel) % 64, h = (V[y] + line) % 32;
                    if (video[h][w] == 1) V[0xF] = 1u;
                    video[h][w] ^= 1u;
                }
            }
        }
    }
    if ((inst & INST_F0FF) == SKP) {
        if (V[x] == pressedKey) {
            PC += 2;
        }
    }
    if ((inst & INST_F0FF) == SKNP) {
        if (V[x] != pressedKey) {
            PC += 2;
        }
    }
    if ((inst & INST_F0FF) == LD_VDT) {
        V[x] = DT;
    }
    if ((inst & INST_F0FF) == LD_VK) {
        waitingX = x;
    }
    if ((inst & INST_F0FF) == LD_DTV) {
        DT = V[x];
    }
    if ((inst & INST_F0FF) == LD_STV) {
        ST = V[x];
    }
    if ((inst & INST_F0FF) == ADD_IV) {
        I = I + V[x];
    }
    if ((inst & INST_F0FF) == LD_FV) {
        I = V[x] * 5;
    }
    //    if ((inst & INST_F0FF) == 0xF030) {
    //        return fmt::format("{:04x} - LD     HF, V{}", i, x);
    //    }
    if ((inst & INST_F0FF) == BCD_V) {
        const auto value = V[x];
        const auto hundreds = value / 100u;
        const auto tens = (value / 10u) % 10u;
        const auto units = value % 10u;

        memory[I] = hundreds;
        memory[I + 1] = tens;
        memory[I + 2] = units;
    }
    if ((inst & INST_F0FF) == LD_MEMI_V) {
        for (int i = 0; i <= x; i++) {
            memory[I + i] = V[i];
        }
    }
    if ((inst & INST_F0FF) == LD_V_MEMI) {
        for (int i = 0; i <= x; i++) {
            V[i] = memory[I + i];
        }
    }
    PC += 2;
    //    // if ((inst & INST_F0FF) == 0xF075) {
    //    //     return fmt::format("{:04x} - LD     R, V{}", i, x);
    //    // }
    //    // if ((inst & INST_F0FF) == 0xF085) {
    //    //     return fmt::format("{:04x} - LD     V{}, R", i, x);
    //    // }
    //    if ((inst & INST_F0FF) == LD_AV) {
    //        return fmt::format("{:04x} - LD     A, V{}", i, x);
    //    }
}
void Chip8::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::RectangleShape rect(sf::Vector2f(10, 10));
    rect.setFillColor(sf::Color::Green);

    for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 64; ++j) {
            if (video[i][j]) {
                rect.setPosition(j * 10, i * 10);
                target.draw(rect);
            }
        }
    }
}
void Chip8::setReleasedKey() { pressedKey = 0x10; }
void Chip8::finish() { timer_thread.request_stop(); }
}  // namespace Emuze::Chip8