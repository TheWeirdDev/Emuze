//
// Created by alireza on 5/30/20.
//

#include "Chip8.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace Emuze::Chip8 {

Chip8::Chip8() {
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

void Chip8::openRom(std::string file) {
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
    // memory.fill(0);
    V.fill(0);
    video.fill(0);
    std::stack<Uint16>().swap(c8stack);

    waitingX = 0xF;
    romSize = 0;
    SP = I = 0;
    PC = base;
    DT = ST = clock = 0;
}

void inline Chip8::setPressedKey(Uint8 keyCode) {
    if (waitingX != 0xF) {
        waitingX = 0xF;
        V[waitingX] = keyCode;
    }
    pressedKey = keyCode;
}

void Chip8::step() {
    if (waitingX != 0xF) return;
    DT--;
    const unsigned short inst = Uint16(memory[PC]) << 8u | memory[PC + 1];
    // 12-bit literal Address
    const Uint16 a = inst & 0xfffU;

    // byte and nibble literals
    const Uint8 b = inst & 0xffU;
    const Uint8 n = inst & 0xfU;

    // vx and vy registers
    const Uint8 x = Uint16(inst >> 8U) & 0xfU;
    const Uint8 y = Uint16(inst >> 4U) & 0xfU;
    spdlog::info(Disasm::disassemble(inst, PC));
    // std::return a << b << n << x << y << '\n';
    if (inst == CLS) {
        video.fill(0);
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
        const unsigned result = Uint16(V[x]) + Uint16(V[y]);
        V[0xF] = result >= 0xFFU;
        V[x] = result & 0xFU;
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
        // TODO: random
        V[x] = (rand() % 0xFFU) & b;
    }
    if ((inst & INST_F000) == DRW) {
        V[0xF] = 0;
        for (int line = 0; line < n; line++) {
            const auto data = memory[I + line];
            auto currentPixel = 7u;
            for (int pixel = 0; pixel < 8; pixel++, currentPixel--) {
                const auto mask = 1u << currentPixel;
                if (data & mask) {
                    auto pos = (V[y] + line) * 8u + V[x] / 8u;
                    if (pixel + (V[x] % 8) > 7) pos++;
                    const auto mask2 = 1u
                                       << (7u - ((pixel + (V[x] % 8u)) % 8u));
                    if ((video[pos] & mask2) == 1) V[0xF] = 1u;  // collision
                    video[pos] |= (video[pos] & mask2) ^ (0xffu & mask2);
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
        const auto hundreds = value / 100;
        const auto tens = (value / 10) % 10;
        const auto units = value % 10;

        memory[I] = hundreds;
        memory[I + 1] = tens;
        memory[I + 2] = units;
    }
    if ((inst & INST_F0FF) == LD_MEMI_V) {
        for (int i = 0; i < x; i++) {
            memory[I + i] = V[i];
        }
    }
    if ((inst & INST_F0FF) == LD_V_MEMI) {
        for (int i = 0; i < x; i++) {
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
    for (int y = 0; const auto& bytes : video) {
        auto currentPixel = 7u;
        for (int i = 0; i < 8; i++, currentPixel--) {
            const auto mask = 1u << currentPixel;
            if (bytes & mask) {
                sf::RectangleShape rect(sf::Vector2f(10, 10));
                rect.setFillColor(sf::Color::Green);
                rect.setPosition(((y % 8) * 8 + i) * 10, (y / 8) * 10);
                target.draw(rect);
            }
        }
        y++;
    }
}
}  // namespace Emuze::Chip8