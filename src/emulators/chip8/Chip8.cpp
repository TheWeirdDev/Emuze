//
// Created by alireza on 5/30/20.
//

#include "Chip8.h"

namespace Emuze::Chip8 {

Chip8::Chip8() {
    if (!forward_font.loadFromFile("../assets/FFFFORWARD.TTF")) {
        spdlog::error("Can't load font file");
        std::exit(1);
    }
}

Chip8::Chip8Sound::Chip8Sound() {
    std::vector<sf::Int16> samples{};
    constexpr double TWO_PI = M_PI * 2;
    constexpr double increment = 440. / 44100;
    double x = 0;
    samples.reserve(SAMPLES);
    for (size_t i = 0; i < SAMPLES; i++) {
        samples[i] = static_cast<sf::Int16>(AMPLITUDE * sin(x * TWO_PI));
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
    if (!in_file.read((char*)(memory.data() + 0x200),
                      static_cast<std::streamsize>(fileSize))) {
        spdlog::error("Error while reading the rom file");
        std::exit(1);
    }
    currentRom = file;
    finished = false;
    for (unsigned int i = 0; i < fileSize; ++i) {
        const auto address = i + 0x200;
        disasm.insert(std::make_pair(address, Disasm::disassemble(sf::Uint16(memory[address]) << 8u | memory[address + 1], address)));
    }
}
void Chip8::reset() {
    memory.fill(0);
    disasm.clear();
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
    debug = false;
}

void Chip8::setPressedKey(sf::Keyboard::Key keyCode) {
    switch (keyCode) {
        case sf::Keyboard::R: {
            openRom(currentRom);
            return;
        }
        case sf::Keyboard::M: {
            finished = true;
            reset();
            return;
        }
        case sf::Keyboard::B: {
            debug = !debug;
            if (debug)
                sound.stop();

            break;
        }
        case sf::Keyboard::L: {
            if (debug) {
                step();
            }
            break;   
        }
        default:
            break;
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

    if (DT > 0) {
        DT--;
    }
    if (ST > 0) {
        sound.play();
        ST--;
        if (ST == 0) sound.stop();
    }

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
        DT = V[x] * 16;
    } else if ((inst & INST_F0FF) == LD_STV) {
        ST = V[x] * 16;
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
    PC += INSTRUCTION_LENGTH;
}
void Chip8::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::Text text;
    text.setFillColor(sf::Color::Green);
    text.setFont(forward_font);
    text.setString("[R] Reset\t[M] Main Menu\t[B] Debug\t[L] Step");
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
    
    if (debug){
        constexpr auto STEP = 2;
        auto start = std::max((int)base, PC - 5 * STEP);
        for(int i = start, j = 0; i < start + 11 * STEP; i += STEP, j++) {
            auto color = sf::Color::Green; 
            if (j == 5) {
                sf::RectangleShape rect(sf::Vector2f(32*PIXEL_SCALE+8, 2*PIXEL_SCALE+8));
                rect.setFillColor(sf::Color(110,110,110));               
                rect.setPosition(sf::Vector2f(PIXEL_SCALE * (DISPLAY_COLUMNS + 4) - 32, j*(DISPLAY_ROWS)));
                target.draw(rect);
                color = sf::Color::White;
            }
            text.setFillColor(color);
            
            text.setString(disasm.at(i));
            text.setCharacterSize(16);
            text.setPosition(sf::Vector2f(PIXEL_SCALE * (DISPLAY_COLUMNS + 4), j*(DISPLAY_ROWS)+8));
            target.draw(text);
        }

        const auto print_register = [&](auto reg, auto reg_name, auto posx, auto posy) {
            text.setString(fmt::format("{}: {} ({:#04x})", reg_name, reg, reg)); 
            text.setPosition(sf::Vector2f(PIXEL_SCALE * posx, PIXEL_SCALE * (DISPLAY_ROWS + posy)));
            target.draw(text);
        };

        text.setFillColor(sf::Color::White);
        print_register(PC,"PC", 2,  2);
        print_register(I, "I" , 24, 2);
        print_register(DT,"DT", 2,  6);
        print_register(ST,"ST", 24, 6);

        text.setFillColor(sf::Color(160,160,160));
        int counter = 0;
        for (const auto& i : V) {
            print_register(i, fmt::format("V{:0X}", counter), (counter % 4) * 16 + 2, 14 + (counter / 4) * 3);
            counter++;
        }
    }
}

void Chip8::setReleasedKey() { pressedKey = UNSET_KEY; }
void Chip8::finish() { finished = true; }

}  // namespace Emuze::Chip8
