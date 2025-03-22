#include "Headers/Utils.hpp"
#include <iostream>
#include <vector>
#include "Headers/InstructionFile.hpp"

void readMachineCode(const char* fileName) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        std::cerr << "Error: Unable to open file " << fileName << std::endl;
        exit(1);
    } //debug

    uint32_t address, instruction;
    
    while (fscanf(file, "%x: %x", &address, &instruction) == 2) {
        instructionMemory.push_back(instruction); // Store at correct index
    }

    fclose(file);
}

uint32_t getInstruction(int address) {
    return instructionMemory[address / 4];
}

void outputStageandCycles() {
    //make a file in folder outputfiles named filename_noforward_out.txt
//     Use ; as a delimiter between cycles within a line. Thus output corresponding to the first two rows of the
//      nonforwarding processor will look as follows:
//      addi x5 x0 0;IF;ID;EX;MEM;WB
//      add x6 x5 x10; ;IF;ID;-;-;EX;MEM;WB


//go through each instruction, access it's vector of strings and print the string as mentioned above

}

bool checkDataHazard(Instruction instruction1, Instruction instruction2) {
    
    return false;
}