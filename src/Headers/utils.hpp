#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdint.h>

void readMachineCode(const char* fileName);
uint32_t getInstruction(int address);
void outputStageandCycles();
bool checkDataHazard(Instruction instruction1, Instruction instruction2);
#endif // UTILS_HPP