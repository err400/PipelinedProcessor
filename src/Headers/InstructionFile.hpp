#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include <vector>

std::vector<uint32_t> instructionMemory; // Empty initially

struct Instruction {
    uint8_t func7;
    uint8_t rs2;
    uint8_t rs1;
    uint8_t func3;
    uint8_t rd;
    uint8_t opcode;

    Instruction(){
        func7 = 0;
        rs2 = 0;
        rs1 = 0;
        func3 = 0;
        rd = 0;
        opcode = 0;
    }
};

void readMachineCode(const char* fileName);
uint32_t getInstruction(int address);

#endif // INSTRUCTION_H