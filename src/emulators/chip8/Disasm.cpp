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
        return fmt::format("{:04x}:\t\tCLS", i);
    }
    if (inst == RET) {
        return fmt::format("{:04x}:\t\tRET", i);
    }
    /*
    if (inst == LOW) {
        return fmt::format("{:04x}:\t\tLOW", i);
    }
    if (inst == HIGH) {
        return fmt::format("{:04x}:\t\tHIGH", i);
    }
    if (inst == SCR) {
        return fmt::format("{:04x}:\t\tSCR", i);
    }
    if (inst == SCL) {
        return fmt::format("{:04x}:\t\tSCL", i);
    }
    if (inst == EXIT) {
        return fmt::format("{:04x}:\t\tEXIT", i);
    }
    if ((inst & 0xFFF0) == 0x00B0) {
        return fmt::format("{:04x}:\t\tSCU    {}", i, n);
    }
    if (((inst & 0xFFF0)) == 0x00C0) {
        return fmt::format("{:04x}:\t\tSCD    {}", i, n);
    }*/
    if ((inst & INST_F000) == SYS) {
        return fmt::format("{:04x}:\t\tSYS    #{}", i, a);
    }
    if ((inst & INST_F000) == JP_X) {
        return fmt::format("{:04x}:\t\tJP     #{:04x}", i, a);
    }
    if ((inst & INST_F000) == CALL) {
        return fmt::format("{:04x}:\t\tCALL   #{:04x}", i, a);
    }
    if ((inst & INST_F000) == SE_VX) {
        return fmt::format("{:04x}:\t\tSE     V{}, #{}", i, x, b);
    }
    if ((inst & INST_F000) == SNE_VX) {
        return fmt::format("{:04x}:\t\tSNE    V{}, #{}", i, x, b);
    }
    if ((inst & INST_F00F) == SE_VV) {
        return fmt::format("{:04x}:\t\tSE     V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == SGT) {
        return fmt::format("{:04x}:\t\tSGT    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == SLT) {
        return fmt::format("{:04x}:\t\tSLT    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F000) == LD_VX) {
        return fmt::format("{:04x}:\t\tLD     V{}, #{}", i, x, b);
    }
    if ((inst & INST_F000) == ADD_VX) {
        return fmt::format("{:04x}:\t\tADD    V{}, #{}", i, x, b);
    }
    if ((inst & INST_F00F) == LD_VV) {
        return fmt::format("{:04x}:\t\tLD     V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == OR) {
        return fmt::format("{:04x}:\t\tOR     V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == AND) {
        return fmt::format("{:04x}:\t\tAND    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == XOR) {
        return fmt::format("{:04x}:\t\tXOR    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == ADD_VV) {
        return fmt::format("{:04x}:\t\tADD    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == SUB) {
        return fmt::format("{:04x}:\t\tSUB    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == SHR) {
        return fmt::format("{:04x}:\t\tSHR    V{}", i, x);
    }
    if ((inst & INST_F00F) == SUBN) {
        return fmt::format("{:04x}:\t\tSUBN   V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == SHL) {
        return fmt::format("{:04x}:\t\tSHL    V{}", i, x);
    }
    if ((inst & INST_F00F) == SNE_VV) {
        return fmt::format("{:04x}:\t\tSNE    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == MUL) {
        return fmt::format("{:04x}:\t\tMUL    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == DIV) {
        return fmt::format("{:04x}:\t\tDIV    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F00F) == BCD_VV) {
        return fmt::format("{:04x}:\t\tBCD    V{}, V{}", i, x, y);
    }
    if ((inst & INST_F000) == LD_IX) {
        return fmt::format("{:04x}:\t\tLD     I, #{}", i, a);
    }
    if ((inst & INST_F000) == JP_V0X) {
        return fmt::format("{:04x}:\t\tJP     V0, #{}", i, a);
    }
    if ((inst & INST_F000) == RND) {
        return fmt::format("{:04x}:\t\tRND    V{}, #{}", i, x, b);
    }
    if ((inst & INST_F000) == DRW) {
        return fmt::format("{:04x}:\t\tDRW    V{}, V{}, {}", i, x, y, n);
    }
    if ((inst & INST_F0FF) == SKP) {
        return fmt::format("{:04x}:\t\tSKP    V{}", i, x);
    }
    if ((inst & INST_F0FF) == SKNP) {
        return fmt::format("{:04x}:\t\tSKNP   V{}", i, x);
    }
    if ((inst & INST_F0FF) == LD_VDT) {
        return fmt::format("{:04x}:\t\tLD     V{}, DT", i, x);
    }
    if ((inst & INST_F0FF) == LD_VK) {
        return fmt::format("{:04x}:\t\tLD     V{}, K", i, x);
    }
    if ((inst & INST_F0FF) == LD_DTV) {
        return fmt::format("{:04x}:\t\tLD     DT, V{}", i, x);
    }
    if ((inst & INST_F0FF) == LD_STV) {
        return fmt::format("{:04x}:\t\tLD     ST, V{}", i, x);
    }
    if ((inst & INST_F0FF) == ADD_IV) {
        return fmt::format("{:04x}:\t\tADD    I, V{}", i, x);
    }
    if ((inst & INST_F0FF) == LD_FV) {
        return fmt::format("{:04x}:\t\tLD     F, V{}", i, x);
    }
    // if ((inst & INST_F0FF) == 0xF030) {
    //     return fmt::format("{:04x}:\t\tLD     HF, V{}", i, x);
    // }
    if ((inst & INST_F0FF) == BCD_V) {
        return fmt::format("{:04x}:\t\tBCD    V{}", i, x);
    }
    if ((inst & INST_F0FF) == LD_MEMI_V) {
        return fmt::format("{:04x}:\t\tLD     [I], V{}", i, x);
    }
    if ((inst & INST_F0FF) == LD_V_MEMI) {
        return fmt::format("{:04x}:\t\tLD     V{}, [I]", i, x);
    }
    // if ((inst & INST_F0FF) == 0xF075) {
    //     return fmt::format("{:04x}:\t\tLD     R, V{}", i, x);
    // }
    // if ((inst & INST_F0FF) == 0xF085) {
    //     return fmt::format("{:04x}:\t\tLD     V{}, R", i, x);
    // }
    if ((inst & INST_F0FF) == LD_AV) {
        return fmt::format("{:04x}:\t\tLD     A, V{}", i, x);
    }
    return fmt::format("{:04x} INVALID", i);
}
}  // namespace Emuze::Chip8
