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

std::string Disasm::disassemble(sf::Uint16 inst) {
    using sf::Uint8, sf::Uint16;
    // 12-bit literal Address
    const Uint16 a = inst & 0xfffU;

    // byte and nibble literals
    const Uint8 b = inst & 0xffU;
    const Uint8 n = inst & 0xfU;

    // vx and vy registers
    const Uint8 x = Uint16(inst >> 8U) & 0xfU;
    const Uint8 y = Uint16(inst >> 4U) & 0xfU;

    //std::return a << b << n << x << y << '\n';
    if (inst == CLS) {
        return fmt::format("{} - CLS", inst);
    }
    if (inst == RET) {
        return fmt::format("{} - RET", inst);
    }
    /*
    if (inst == LOW) {
        return fmt::format("{} - LOW", inst);
    }
    if (inst == HIGH) {
        return fmt::format("{} - HIGH", inst);
    }
    if (inst == SCR) {
        return fmt::format("{} - SCR", inst);
    }
    if (inst == SCL) {
        return fmt::format("{} - SCL", inst);
    }
    if (inst == EXIT) {
        return fmt::format("{} - EXIT", inst);
    }
    if ((inst & 0xFFF0) == 0x00B0) {
        return fmt::format("{} - SCU    {}", inst, n);
    }
    if (((inst & 0xFFF0)) == 0x00C0) {
        return fmt::format("{} - SCD    {}", inst, n);
    }*/
    if ((inst & INST_F000) == SYS) {
        return fmt::format("{} - SYS    #{}", inst, a);
    }
    if ((inst & INST_F000) == JP_X) {
        return fmt::format("{} - JP     #{}", inst, a);
    }
    if ((inst & INST_F000) == CALL) {
        return fmt::format("{} - CALL   #{}", inst, a);
    }
    if ((inst & INST_F000) == SE_VX) {
        return fmt::format("{} - SE     V{}, #{}", inst, x, b);
    }
    if ((inst & INST_F000) == SNE_VX) {
        return fmt::format("{} - SNE    V{}, #{}", inst, x, b);
    }
    if ((inst & INST_F00F) == SE_VV) {
        return fmt::format("{} - SE     V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F00F) == SGT) {
        return fmt::format("{} - SGT    V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F00F) == SLT) {
        return fmt::format("{} - SLT    V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F000) == LD_VX) {
        return fmt::format("{} - LD     V{}, #{}", inst, x, b);
    }
    if ((inst & INST_F000) == ADD_VX) {
        return fmt::format("{} - ADD    V{}, #{}", inst, x, b);
    }
    if ((inst & INST_F00F) == LD_VV) {
        return fmt::format("{} - LD     V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F00F) == OR) {
        return fmt::format("{} - OR     V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F00F) == AND) {
        return fmt::format("{} - AND    V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F00F) == XOR) {
        return fmt::format("{} - XOR    V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F00F) == ADD_VV) {
        return fmt::format("{} - ADD    V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F00F) == SUB) {
        return fmt::format("{} - SUB    V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F00F) == SHR) {
        return fmt::format("{} - SHR    V{}", inst, x);
    }
    if ((inst & INST_F00F) == SUBN) {
        return fmt::format("{} - SUBN   V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F00F) == SHL) {
        return fmt::format("{} - SHL    V{}", inst, x);
    }
    if ((inst & INST_F00F) == SNE_VV) {
        return fmt::format("{} - SNE    V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F00F) == MUL) {
        return fmt::format("{} - MUL    V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F00F) == DIV) {
        return fmt::format("{} - DIV    V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F00F) == BCD_VV) {
        return fmt::format("{} - BCD    V{}, V{}", inst, x, y);
    }
    if ((inst & INST_F000) == LD_IX) {
        return fmt::format("{} - LD     I, #{}", inst, a);
    }
    if ((inst & INST_F000) == JP_V0X) {
        return fmt::format("{} - JP     V0, #{}", inst, a);
    }
    if ((inst & INST_F000) == RND) {
        return fmt::format("{} - RND    V{}, #{}", inst, x, b);
    }
    if ((inst & INST_F000) == DRW) {
        return fmt::format("{} - DRW    V{}, V{}, {}", inst, x, y, n);
    }
    if ((inst & INST_F0FF) == SKP) {
        return fmt::format("{} - SKP    V{}", inst, x);
    }
    if ((inst & INST_F0FF) == SKNP) {
        return fmt::format("{} - SKNP   V{}", inst, x);
    }
    if ((inst & INST_F0FF) == LD_VDT) {
        return fmt::format("{} - LD     V{}, DT", inst, x);
    }
    if ((inst & INST_F0FF) == LD_VK) {
        return fmt::format("{} - LD     V{}, K", inst, x);
    }
    if ((inst & INST_F0FF) == LD_DTV) {
        return fmt::format("{} - LD     DT, V{}", inst, x);
    }
    if ((inst & INST_F0FF) == LD_STV) {
        return fmt::format("{} - LD     ST, V{}", inst, x);
    }
    if ((inst & INST_F0FF) == ADD_IV) {
        return fmt::format("{} - ADD    I, V{}", inst, x);
    }
    if ((inst & INST_F0FF) == LD_FV) {
        return fmt::format("{} - LD     F, V{}", inst, x);
    }
    // if ((inst & INST_F0FF) == 0xF030) {
    //     return fmt::format("{} - LD     HF, V{}", inst, x);
    // }
    if ((inst & INST_F0FF) == BCD_V) {
        return fmt::format("{} - BCD    V{}", inst, x);
    }
    if ((inst & INST_F0FF) == LD_MEMI_V) {
        return fmt::format("{} - LD     [I], V{}", inst, x);
    }
    if ((inst & INST_F0FF) == LD_V_MEMI) {
        return fmt::format("{} - LD     V{}, [I]", inst, x);
    }
    // if ((inst & INST_F0FF) == 0xF075) {
    //     return fmt::format("{} - LD     R, V{}", inst, x);
    // }
    // if ((inst & INST_F0FF) == 0xF085) {
    //     return fmt::format("{} - LD     V{}, R", inst, x);
    // }
    if ((inst & INST_F0FF) == LD_AV) {
        return fmt::format("{} - LD     A, V{}", inst, x);
    }
    return "INVALID";
}
}  // namespace Emuze::Chip8