#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

struct Instruction {
    uint8_t func7;
    uint8_t rs2;
    uint8_t rs1;
    uint8_t func3;
    uint8_t rd;
    uint8_t opcode;
};

#endif // INSTRUCTION_H