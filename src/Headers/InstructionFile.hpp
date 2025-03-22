#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include <vector>

std::vector<uint32_t> instructionMemory; // Empty initially

enum class Instruction_type{
    R_TYPE,
    I_TYPE,
    S_TYPE,
    SB_TYPE,
    UJ_TYPE,
    U_TYPE
};

struct control_signals{
    bool is_branch;
    bool MemRead;
    bool MemtoReg;
    // int32_t AluOp;  // debug
    bool MemWrite;
    bool AluSrc;
    bool RegWrite;
};

struct Instruction {
    uint32_t raw;
    uint8_t func7;
    uint8_t rs2;
    uint8_t rs1;
    uint8_t func3;
    uint8_t rd;
    uint8_t opcode;
    // no two imm in case of stores
    uint32_t imm;
    Instruction_type type;
    control_signals controls;
};

void decodeInstruction(Instruction& instr);

#endif // INSTRUCTION_H