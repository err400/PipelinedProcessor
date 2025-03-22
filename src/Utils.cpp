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