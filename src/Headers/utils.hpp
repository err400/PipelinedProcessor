#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdint.h>
#include "InstructionFile.hpp"
#include "PipelineStages.hpp"

void readMachineCode(const char* fileName);
Instruction* getInstruction(int address);
void outputStageandCycles(const string& filename);
bool checkDataHazardrs1(Instruction* instruction1, Instruction* instruction2);
bool checkDataHazardrs2(Instruction* instruction1, Instruction* instruction2);
void forward(IFStageData* if_stage, IDStageData* id_stage, EXStageData* ex_stage, MEMStageData* mem_stage, WBStageData* wb_stage);
#endif // UTILS_HPP