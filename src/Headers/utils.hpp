#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdint.h>
#include "InstructionFile.hpp"

void readMachineCode(const char* fileName);
Instruction* getInstruction(int address);
void outputStageandCycles(const string& filename);
bool checkDataHazard(Instruction* instruction1, Instruction* instruction2);
#endif // UTILS_HPP