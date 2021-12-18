#ifndef EMUZE_CHIP8DISASM_H
#define EMUZE_CHIP8DISASM_H

#include <fmt/format.h>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <iostream>
#include <string_view>

namespace Emuze::Chip8 {

class Disasm {

   public:
    static std::string disassemble(sf::Uint16 inst, std::size_t i);
};

enum Mask {
    INST_F000 = 0xf000U,
    INST_F00F = 0xf00fU,
    INST_F0FF = 0xf0ffU
};
enum Inst {
    CLS = 0x00E0,
    RET = 0x00EE,
    LOW = 0x00FE,
    HIGH = 0x00FF,
    SCR = 0x00FB,
    SCL = 0x00FC,
    EXIT = 0x00FD,
    SYS = 0x0000,
    JP_X = 0x1000,
    CALL = 0x2000,
    SE_VX = 0x3000,
    SNE_VX = 0x4000,
    SE_VV = 0x5000,
    SGT = 0x5001,
    SLT = 0x5002,
    LD_VX = 0x6000,
    ADD_VX = 0x7000,
    LD_VV = 0x8000,
    OR = 0x8001,
    AND = 0x8002,
    XOR = 0x8003,
    ADD_VV = 0x8004,
    SUB = 0x8005,
    SHR = 0x8006,
    SUBN = 0x8007,
    SHL = 0x800E,
    SNE_VV = 0x9000,
    MUL = 0x9001,
    DIV = 0x9002,
    BCD_VV = 0x9003,
    LD_IX = 0xA000,
    JP_V0X = 0xB000,
    RND = 0xC000,
    SKP = 0xE09E,
    SKNP = 0xE0A1,
    DRW = 0xD000,
    LD_VDT = 0xF007,
    LD_VK = 0xF00A,
    LD_DTV = 0xF015,
    LD_STV = 0xF018,
    ADD_IV = 0xF01E,
    LD_FV = 0xF029,
    BCD_V = 0xF033,
    LD_MEMI_V = 0xF055,
    LD_V_MEMI = 0xF065,
    LD_AV = 0xF094,
};
}  // namespace Emuze::Chip8

#endif  //EMUZE_CHIP8DISASM_H
