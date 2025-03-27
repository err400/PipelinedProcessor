#include "Headers/Utils.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include<iomanip>
#include <filesystem>
using namespace std;
namespace fs = std::__fs::filesystem;

void readMachineCode(const char* fileName) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        std::cerr << "Error: Unable to open file " << fileName << std::endl; //debug
        exit(1);
    }
    char line[100]; //debug
    while (fgets(line, sizeof(line), file)) {
        uint32_t address;
        uint32_t instruction;
        char instructionStr[128] = {0};
        if (sscanf(line, "%x: %x %[^\n]", &address, &instruction, instructionStr) >= 2) {
            Instruction instr;
            instr.raw = instruction;
            instr.instStr = std::string(instructionStr);
            instructionMemory.push_back(instr);
        }
    }
    
    fclose(file);
}

Instruction* getInstruction(int address) {
    // instructionMemory[address / 4].idx = address / 4;  // debug
    return &(instructionMemory[address / 4]);
}

void outputStageandCycles(const string& filename) {
    // make a file in folder outputfiles named filename_noforward_out.txt
    // Use ; as a delimiter between cycles within a line. Thus output corresponding to the first two rows of the
    // nonforwarding processor will look as follows:
    // addi x5 x0 0;IF;ID;EX;MEM;WB
    // add x6 x5 x10; ;IF;ID;-;-;EX;MEM;WB
    fs::path path_obj(filename);
    string base_filename = path_obj.filename(); 
    string file_dest;
    // if(is_forwarded){
    //     file_dest = "../outputfiles/" + base_filename.substr(0, base_filename.find_last_of('.')) + "_forward_out.txt";
    // }
    // else{
    //     file_dest = "../outputfiles/" + base_filename.substr(0, base_filename.find_last_of('.')) + "_noforward_out.txt";
    // }
    file_dest = "../outputfiles/" + base_filename.substr(0, base_filename.find_last_of('.')) + "_noforward_out.txt";
    ofstream outputFile(file_dest);
    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open file " << file_dest << endl;
        exit(1);
    }
    int n = instructionMemory.size();
    for(int i = 0;i<n;i++){
        Instruction instr = instructionMemory[i];
        outputFile<<left << setw(20)<<instr.instStr<<";  ";
        // print the instruction
        for(auto it: instr.vec){
            outputFile<< setw(3)<<it<<";";
        }
        outputFile<<endl;
    }
    outputFile.close();
    // go through each instruction, access it's vector of strings and print the string as mentioned above

}

bool checkDataHazardrs1(Instruction* instruction1, Instruction* instruction2) {
    int opcode1 = instruction1->opcode;
    // int opcode2 = instruction2.opcode;  // debug
    // sw - no stalls
    if(instruction2->controls.MemWrite){
        return false;
    }
    // Check if instruction1 is a JAL,LUI or AUIPC
    if (instruction1->type == Instruction_type::UJ_TYPE || instruction1->type == Instruction_type::U_TYPE)
        return false; // no rs1
    if (instruction2->type == Instruction_type::S_TYPE || instruction2->type == Instruction_type::SB_TYPE)
        return false; // no rd
    int rs1 = instruction1->rs1;
    printf("rs1: %d\n", rs1); // debug
    int rd = instruction2->rd;
    
    if (rs1 == rd)
        return true;
    
    return false;
}

bool checkDataHazardrs2(Instruction* instruction1, Instruction* instruction2) {
    int opcode1 = instruction1->opcode;
    // int opcode2 = instruction2.opcode;  // debug
    // sw - no stalls
    if(instruction2->controls.MemWrite){
        return false;
    }
    // Check if instruction1 is a JAL,LUI or AUIPC, no Immediates
    if (instruction1->type == Instruction_type::UJ_TYPE || instruction1->type == Instruction_type::U_TYPE || instruction1->type == Instruction_type::I_TYPE) 
        return false; // no rs2
    if (instruction2->type == Instruction_type::S_TYPE || instruction2->type == Instruction_type::SB_TYPE)
        return false; //no rd
    int rs2 = instruction1->rs2;
    int rd = instruction2->rd;
    
    if (rs2 == rd)
        return true;
    
    return false;
}