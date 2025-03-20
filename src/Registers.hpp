#ifndef REGISTERS_H
#define REGISTERS_H

#include <array>
#include <stdint.h>

class Registers{
private:
    // The 32 bit registers are stored in an array
    std::array<int32_t, 32> registers;

public:
    Registers();
    uint32_t readRegister(int regNum);
    void writeRegister(int regNum, uint32_t value);
};

#endif // REGISTERS_H