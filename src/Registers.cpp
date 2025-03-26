#include "Registers.hpp"
#include <algorithm>
#include <cstdio>

Registers::Registers(){
    std::fill(registers.begin(), registers.end(), 0);
}

int32_t Registers::readRegister(int regNum){
    if (regNum == 0){
        return 0; // Register x0 is hardwired to 0
    }
    return registers[regNum];
}

void Registers::writeRegister(int regNum, uint32_t value){
    if (regNum == 0){
        printf("Warning: Attempted to write to register x0\n"); //Debug
        return; // Register x0 is hardwired to 0
    }
    registers[regNum] = value;
}