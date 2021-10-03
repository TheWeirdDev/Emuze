//
// Created by alireza on 5/30/20.
//

#include "Chip8.h"

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
      }) {}

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
        spdlog::error("Failed to load sound samples");
        throw std::exception();
    }
    sound.setBuffer(buffer);
    sound.setLoop(true);
}

void Chip8::openRom(const std::string& file) {
    reset();

    std::ifstream in_file(file, std::ios::in | std::ios::binary);

    const auto fileSize = std::filesystem::file_size(file);
    if (fileSize >= MEM_SIZE - 0x200) {
        spdlog::error("Rom file is too big");
        std::exit(1);
    }
    if (!in_file.read((char*)(memory.data() + 0x200), fileSize)) {
        spdlog::error("Error while reading the rom file");
        std::exit(1);
    }
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
    for (int index = 0; const auto& i : digits) {
        for (const auto& j : i) {
            memory[index++] = j;
        }
    }
    V.fill(0);
    for (auto& i : video) i.fill(false);
    std::stack<Uint16>().swap(c8stack);

    waitingX = UNSET_KEY;
    pressedKey = UNSET_KEY;
    I = 0;
    PC = base;
    DT = ST = 0;
}

void Chip8::setPressedKey(sf::Keyboard::Key keyCode) {
    if (keyCode == sf::Keyboard::R) {
        reset();
        return;
    }

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
        if (waitingX != UNSET_KEY) {
            V[waitingX] = keys.at(keyCode);
            waitingX = UNSET_KEY;
        }
        pressedKey = keys.at(keyCode);
    }
}

void Chip8::step() {
    if (waitingX != UNSET_KEY) return;

    const unsigned short inst = Uint16(memory[PC]) << 8u | memory[PC + 1];
    // 12-bit literal Address
    const Uint16 a = inst & 0xfffU;

    // byte and nibble literals
    const Uint8 b = inst & 0xffU;
    const Uint8 n = inst & 0xfU;

    // vx and vy registers
    const Uint8 x = Uint16(inst >> 8U) & 0xfU;
    const Uint8 y = Uint16(inst >> 4U) & 0xfU;

    if (inst == CLS) {
        for (auto& i : video) i.fill(false);
    } else if (inst == RET) {
        PC = c8stack.top();
        c8stack.pop();
    } else if ((inst & INST_F000) == JP_X) {
        PC = a;
        return;
    } else if ((inst & INST_F000) == CALL) {
        c8stack.push(PC);
        PC = a;
        return;
    } else if ((inst & INST_F000) == SE_VX) {
        if (V[x] == b) {
            PC += 2;
        }
    } else if ((inst & INST_F000) == SNE_VX) {
        if (V[x] != b) {
            PC += 2;
        }
    } else if ((inst & INST_F00F) == SE_VV) {
        if (V[x] == V[y]) {
            PC += 2;
        }
    } else if ((inst & INST_F000) == LD_VX) {
        V[x] = b;
    } else if ((inst & INST_F000) == ADD_VX) {
        V[x] += b;
    } else if ((inst & INST_F00F) == LD_VV) {
        V[x] = V[y];
    } else if ((inst & INST_F00F) == OR) {
        V[x] = V[x] | V[y];
    } else if ((inst & INST_F00F) == AND) {
        V[x] = V[x] & V[y];
    } else if ((inst & INST_F00F) == XOR) {
        V[x] = V[x] ^ V[y];
    } else if ((inst & INST_F00F) == ADD_VV) {
        const Uint16 result = Uint16(V[x]) + Uint16(V[y]);
        V[0xF] = result >= 0xFFU;
        V[x] = result & 0xFFU;
    } else if ((inst & INST_F00F) == SUB) {
        V[0xF] = V[x] > V[y];
        V[x] -= V[y];
    } else if ((inst & INST_F00F) == SHR) {
        V[0xF] = V[x] & 0x1u;
        V[x] >>= 1u;
    } else if ((inst & INST_F00F) == SUBN) {
        V[0xF] = V[y] > V[x];
        V[x] = V[y] - V[x];
    } else if ((inst & INST_F00F) == SHL) {
        V[0xF] = V[x] & 0x8u;
        V[x] <<= 1u;
    } else if ((inst & INST_F00F) == SNE_VV) {
        if (V[x] != V[y]) {
            PC += 2;
        }
    } else if ((inst & INST_F000) == LD_IX) {
        I = a;
    } else if ((inst & INST_F000) == JP_V0X) {
        PC = V[0] + a;
        return;
    } else if ((inst & INST_F000) == RND) {
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution dist255(1, 255);
        V[x] = dist255(rng) & b;
    } else if ((inst & INST_F000) == DRW) {
        V[0xF] = 0;
        for (int line = 0; line < n; line++) {
            const auto data = memory[I + line];
            auto currentPixel = 7u;
            for (int pixel = 0; pixel < 8; pixel++, currentPixel--) {
                const auto mask = 1u << currentPixel;
                if (data & mask) {
                    const auto w = (V[x] + pixel) % DISPLAY_COLUMNS,
                               h = (V[y] + line) % DISPLAY_ROWS;
                    if (video[h][w] == 1) V[0xF] = 1u;
                    video[h][w] ^= 1u;
                }
            }
        }
    } else if ((inst & INST_F0FF) == SKP) {
        if (V[x] == pressedKey) {
            PC += 2;
        }
    } else if ((inst & INST_F0FF) == SKNP) {
        if (V[x] != pressedKey) {
            PC += 2;
        }
    } else if ((inst & INST_F0FF) == LD_VDT) {
        V[x] = DT;
    } else if ((inst & INST_F0FF) == LD_VK) {
        waitingX = x;
    } else if ((inst & INST_F0FF) == LD_DTV) {
        DT = V[x];
    } else if ((inst & INST_F0FF) == LD_STV) {
        ST = V[x];
    } else if ((inst & INST_F0FF) == ADD_IV) {
        I = I + V[x];
    } else if ((inst & INST_F0FF) == LD_FV) {
        I = V[x] * 5;
    } else if ((inst & INST_F0FF) == BCD_V) {
        const auto value = V[x];
        const auto hundreds = value / 100u;
        const auto tens = (value / 10u) % 10u;
        const auto units = value % 10u;

        memory[I] = hundreds;
        memory[I + 1] = tens;
        memory[I + 2] = units;
    } else if ((inst & INST_F0FF) == LD_MEMI_V) {
        for (int i = 0; i <= x; i++) {
            memory[I + i] = V[i];
        }
    } else if ((inst & INST_F0FF) == LD_V_MEMI) {
        for (int i = 0; i <= x; i++) {
            V[i] = memory[I + i];
        }
    }
    // Go to the next instruction
    PC += 2;
}
void Chip8::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::Font font;
    font.loadFromFile("../assets/FFFFORWARD.TTF");

    sf::Text text;
    text.setFillColor(sf::Color::Green);
    text.setFont(font);
    text.setString("[R] Reset");
    text.setCharacterSize(22);
    text.setPosition(sf::Vector2f(10, Emuze::SCREEN_HEIGHT - 30));
    target.draw(text);

    sf::RectangleShape rect(sf::Vector2f(PIXEL_SCALE, PIXEL_SCALE));
    rect.setFillColor(sf::Color::Green);

    for (int i = 0; i < DISPLAY_ROWS; ++i) {
        for (int j = 0; j < DISPLAY_COLUMNS; ++j) {
            if (video[i][j]) {
                rect.setPosition(j * PIXEL_SCALE, i * PIXEL_SCALE);
                target.draw(rect);
            }
        }
    }
}
void Chip8::setReleasedKey() { pressedKey = UNSET_KEY; }
void Chip8::finish() { timer_thread.request_stop(); }
}  // namespace Emuze::Chip8