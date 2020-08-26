//
// Created by alireza on 5/24/20.
//

#include "Disasm.h"

#include <SFML/Graphics.hpp>
namespace Emuze::Chip8 {
void Disasm::draw(sf::RenderTarget &target, sf::RenderStates states) const {
    constexpr float size = 200.0F;
    auto r = sf::RectangleShape(sf::Vector2f(size, size));
    r.setFillColor(sf::Color::Blue);
    target.draw(r, states);
}

std::string Disasm::disassemble(sf::Uint16 inst, std::size_t i) {
    using sf::Uint8, sf::Uint16;
    using namespace std::string_literals;
    // 12-bit literal Address
    const Uint16 a = inst & 0xfffU;

    // byte and nibble literals
    const Uint8 b = inst & 0xffU;
    const Uint8 n = inst & 0xfU;

    // vx and vy registers
    const Uint8 x = Uint16(inst >> 8U) & 0xfU;
    const Uint8 y = Uint16(inst >> 4U) & 0xfU;

    // std::return a << b << n << x << y << '\n';
    if (inst == CLS) {
        return fmt::format("{:04x} - CLS", i);
    }
    if (inst == RET) {
        return fmt::format("{:04x} - RET", i);
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
        return fmt::format("{:04x} - SYS    #{}", i, a);
    }
    if ((inst & INST_F000) == JP_X) {
        return fmt::format("{:04x} - JP     #{:04x}", i, a);
    }
    if ((inst & INST_F000) == CALL) {
        return fmt::format("{:04x} - CALL   #{:04x}", i, a);
    }
    if ((inst & INST_F000) == SE_VX) {
        return fmt::format("{:04x} - SE     V{}, #{}", i, x, b);
    }
    if ((inst & INST_F000) == SNE_VX) {
        return fmt::format("{:04x} - SNE    V{}, #{}", i, x, b);
    }
    if ((inst & INST_F00F) == SE_VV) {
        return fmt::format("{:04x} - SE     V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == SGT) {
        return fmt::format("{:04x} - SGT    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == SLT) {
        return fmt::format("{:04x} - SLT    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F000) == LD_VX) {
        return fmt::format("{:04x} - LD     V{}, #{}", i, x, b);
    }
    if ((inst & INST_F000) == ADD_VX) {
        return fmt::format("{:04x} - ADD    V{}, #{}", i, x, b);
    }
    if ((inst & INST_F00F) == LD_VV) {
        return fmt::format("{:04x} - LD     V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == OR) {
        return fmt::format("{:04x} - OR     V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == AND) {
        return fmt::format("{:04x} - AND    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == XOR) {
        return fmt::format("{:04x} - XOR    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == ADD_VV) {
        return fmt::format("{:04x} - ADD    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == SUB) {
        return fmt::format("{:04x} - SUB    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == SHR) {
        return fmt::format("{:04x} - SHR    V{}", i, x);
    }
    if ((inst & INST_F00F) == SUBN) {
        return fmt::format("{:04x} - SUBN   V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == SHL) {
        return fmt::format("{:04x} - SHL    V{}", i, x);
    }
    if ((inst & INST_F00F) == SNE_VV) {
        return fmt::format("{:04x} - SNE    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == MUL) {
        return fmt::format("{:04x} - MUL    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == DIV) {
        return fmt::format("{:04x} - DIV    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == BCD_VV) {
        return fmt::format("{:04x} - BCD    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F000) == LD_IX) {
        return fmt::format("{:04x} - LD     I, #{}", i, a);
    }
    if ((inst & INST_F000) == JP_V0X) {
        return fmt::format("{:04x} - JP     V0, #{}", i, a);
    }
    if ((inst & INST_F000) == RND) {
        return fmt::format("{:04x} - RND    V{}, #{}", i, x, b);
    }
    if ((inst & INST_F000) == DRW) {
        return fmt::format("{:04x} - DRW    V{}, V{}, {}", i, x, y, n);
    }
    if ((inst & INST_F0FF) == SKP) {
        return fmt::format("{:04x} - SKP    V{}", i, x);
    }
    if ((inst & INST_F0FF) == SKNP) {
        return fmt::format("{:04x} - SKNP   V{}", i, x);
    }
    if ((inst & INST_F0FF) == LD_VDT) {
        return fmt::format("{:04x} - LD     V{}, DT", i, x);
    }
    if ((inst & INST_F0FF) == LD_VK) {
        return fmt::format("{:04x} - LD     V{}, K", i, x);
    }
    if ((inst & INST_F0FF) == LD_DTV) {
        return fmt::format("{:04x} - LD     DT, V{}", i, x);
    }
    if ((inst & INST_F0FF) == LD_STV) {
        return fmt::format("{:04x} - LD     ST, V{}", i, x);
    }
    if ((inst & INST_F0FF) == ADD_IV) {
        return fmt::format("{:04x} - ADD    I, V{}", i, x);
    }
    if ((inst & INST_F0FF) == LD_FV) {
        return fmt::format("{:04x} - LD     F, V{}", i, x);
    }
    // if ((inst & INST_F0FF) == 0xF030) {
    //     return fmt::format("{:04x} - LD     HF, V{}", i, x);
    // }
    if ((inst & INST_F0FF) == BCD_V) {
        return fmt::format("{:04x} - BCD    V{}", i, x);
    }
    if ((inst & INST_F0FF) == LD_MEMI_V) {
        return fmt::format("{:04x} - LD     [I], V{}", i, x);
    }
    if ((inst & INST_F0FF) == LD_V_MEMI) {
        return fmt::format("{:04x} - LD     V{}, [I]", i, x);
    }
    // if ((inst & INST_F0FF) == 0xF075) {
    //     return fmt::format("{:04x} - LD     R, V{}", i, x);
    // }
    // if ((inst & INST_F0FF) == 0xF085) {
    //     return fmt::format("{:04x} - LD     V{}, R", i, x);
    // }
    if ((inst & INST_F0FF) == LD_AV) {
        return fmt::format("{:04x} - LD     A, V{}", i, x);
    }
    return fmt::format("{:04x} INVALID", i);
}
}  // namespace Emuze::Chip8
