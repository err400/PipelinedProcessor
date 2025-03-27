#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdint.h>
#include "InstructionFile.hpp"

void readMachineCode(const char* fileName);
Instruction* getInstruction(int address);
void outputStageandCycles(const string& filename);
bool checkDataHazardrs1(Instruction* instruction1, Instruction* instruction2);
bool checkDataHazardrs2(Instruction* instruction1, Instruction* instruction2);
#endif // UTILS_HPP