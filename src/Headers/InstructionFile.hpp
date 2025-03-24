#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include <vector>
#include <string>
using namespace std;


enum class Instruction_type{
    R_TYPE,
    I_TYPE,
    S_TYPE,
    SB_TYPE,
    UJ_TYPE,
    U_TYPE
};

enum class ALUOp{
    ADD,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND,
    MUL,
    // MULH,
    // MULHSU,
    // MULHU,
    DIV,
    // DIVU,
    REM,
    // REMU
};

struct control_signals{
    bool is_branch;
    bool MemRead;
    bool MemtoReg;
    ALUOp AluOp;
    bool MemWrite;
    bool AluSrc;
    bool RegWrite;
    bool is_jump; // zero // debug
};

struct Instruction {
    int idx;  // debug // index of the instruction in the instructionMemory
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
    vector<string> vec;
    string instStr;
};

extern std::vector<Instruction> instructionMemory; // Empty initially

void decodeInstruction(Instruction& instr);

#endif // INSTRUCTION_H