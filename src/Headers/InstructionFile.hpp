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
    DIV,
    REM,
};

struct control_signals{
    bool is_branch;
    bool MemRead;
    bool MemtoReg;
    ALUOp AluOp;
    bool MemWrite;
    bool AluSrc;
    bool RegWrite;
    bool is_jump; // equivalent to zero signal
};

struct Instruction {
    uint32_t raw; //32
    uint8_t func7; //7
    uint8_t rs2; //5
    uint8_t rs1; //5
    uint8_t func3; //3
    uint8_t rd; //5
    uint8_t opcode; //7
    int32_t imm;
    Instruction_type type;
    control_signals controls;
    vector<string> vec;
    string instStr;
};

extern std::vector<Instruction> instructionMemory; // Empty initially

void decodeInstruction(Instruction* instr);

#endif // INSTRUCTION_H