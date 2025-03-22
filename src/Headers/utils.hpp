#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdint.h>

void readMachineCode(const char* fileName);
uint32_t getInstruction(int address);

#endif // UTILS_HPP