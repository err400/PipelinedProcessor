#include "Headers/Utils.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include<iomanip>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;

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

void outputStageandCycles(const string& filename, bool is_forwarded) {
    // make a file in folder outputfiles named filename_noforward_out.txt
    // Use ; as a delimiter between cycles within a line. Thus output corresponding to the first two rows of the
    // nonforwarding processor will look as follows:
    // addi x5 x0 0;IF;ID;EX;MEM;WB
    // add x6 x5 x10; ;IF;ID;-;-;EX;MEM;WB
    fs::path path_obj(filename);
    string base_filename = path_obj.filename().string(); 
    string file_dest;
    if(is_forwarded){
        file_dest = "../outputfiles/" + base_filename.substr(0, base_filename.find_last_of('.')) + "_forward_out.txt";
    }
    else{
        file_dest = "../outputfiles/" + base_filename.substr(0, base_filename.find_last_of('.')) + "_noforward_out.txt";
    }
    // file_dest = "../outputfiles/" + base_filename.substr(0, base_filename.find_last_of('.')) + "_noforward_out.txt";
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

// check for forwarding here
void forward(IFStageData* if_stage, IDStageData* id_stage, EXStageData* ex_stage, MEMStageData* mem_stage, WBStageData* wb_stage){
    // if there is data hazard only in case of lw instruction in ex stage
    // if(ex_stage->instruction != nullptr){
    //     if(ex_stage->instruction->controls.MemtoReg){
    //         if(if_stage->instruction->rs1 == ex_stage->instruction->rd){
    //             id_stage->num_stall = 1;
    //             if_stage->is_first_stalled = true;
    //         }
    //         if(if_stage->instruction->rs2 == ex_stage->instruction->rd){
    //             id_stage->num_stall = 1;
    //             if_stage->is_first_stalled = true;
    //         }
    //     }
    //     // ex and id (normal alu instructions in ex)
    //     else if(ex_stage->instruction->controls.RegWrite){
    //         if(id_stage->instruction->rs1 == ex_stage->instruction->rd){
    //             registers.writeRegister(id_stage->instruction->rs1, ex_stage->alu_output);
    //         }
    //         if(id_stage->instruction->rs2 == ex_stage->instruction->rd){
    //             registers.writeRegister(id_stage->instruction->rs2, ex_stage->alu_output);

    //         }
    //     }
    // }
    // // mem and ex
    // // think of more cases // debug
    // // mem and id
    // // lw and beq case - done
    // // lw in MEM stage - forward
    // if(mem_stage->instruction != nullptr){
    //     if(mem_stage->instruction->controls.MemtoReg){
    //         if(id_stage->instruction->rs1 == mem_stage->instruction->rd){
    //             registers.writeRegister(mem_stage->instruction->rd, mem_stage->mem_read_data);

    //         }
    //         if(id_stage->instruction->rs2 == mem_stage->instruction->rd){
    //             registers.writeRegister(mem_stage->instruction->rd, mem_stage->mem_read_data);
    //         }
    //     }
    // }
    if(if_stage->instruction->type == Instruction_type::R_TYPE || if_stage->instruction->opcode == 0x13){
        //check prev instruction
        if(if_stage->instruction->rs1 == ex_stage->instruction->rd){
            if(ex_stage->instruction->controls.MemRead){
                id_stage->num_stall = 1;
                if_stage->is_first_stalled = true;
            } //ex gets data from mem stage (lw followed by add)
            else{
                id_stage->rs1_readdata = ex_stage->alu_output;
            } //ex gets data from ex stage (add followed by add)
        }
        else if(if_stage->instruction->rs1 == mem_stage->instruction->rd){
            if(mem_stage->instruction->controls.MemRead){
                id_stage->rs1_readdata = mem_stage->mem_read_data;
            } //ex gets data from mem stage (lw followed by NoOp followed by add)
            else{
                id_stage->rs1_readdata = mem_stage->alu_output; //ex gets data from mem stage (add followed by NoOp followed by add)
            }
        }
    }
    else if(if_stage->instruction->type == Instruction_type::R_TYPE){ // pther than immediate types
        if(if_stage->instruction->rs2 == ex_stage->instruction->rd){
            if(ex_stage->instruction->controls.MemRead){
                id_stage->num_stall = 1;
                if_stage->is_first_stalled = true;
            } //ex gets data from mem stage (lw followed by add)
            else{
                id_stage->rs2_readdata = ex_stage->alu_output;
            } //ex gets data from ex stage (add followed by add)
        }
        else if(if_stage->instruction->rs2 == mem_stage->instruction->rd){
            if(mem_stage->instruction->controls.MemRead){
                id_stage->rs2_readdata = mem_stage->mem_read_data;
            } //ex gets data from mem stage (lw followed by NoOp followed by add)
            else{
                id_stage->rs2_readdata = mem_stage->alu_output; //ex gets data from mem stage (add followed by NoOp followed by add)
            }
        }
    }
    else if(if_stage->instruction->opcode == 0x03){ //lw
        // add followed by lw
        if(if_stage->instruction->rs1 == ex_stage->instruction->rd && (ex_stage->instruction->type == Instruction_type::R_TYPE || ex_stage->instruction->opcode == 0x13)){//add and addi type instructions
            id_stage->rs1_readdata = ex_stage->alu_output; //case of address of lw dependent on output
        }
        // add followed by noOp followed by lw
        else if(if_stage->instruction->rs1 == mem_stage->instruction->rd && (ex_stage->instruction->type == Instruction_type::R_TYPE || ex_stage->instruction->opcode == 0x13)){
            id_stage->rs1_readdata = mem_stage->alu_output; //case of address of lw dependent on output
        }
        // lw followed by lw
        else if(if_stage->instruction->rs1 == ex_stage->instruction->rd && ex_stage->instruction->controls.MemRead){
            id_stage->num_stall = 1;
            if_stage->is_first_stalled = true;
        }
        // lw followed by noOp followed by lw
        else if(if_stage->instruction->rs1 == mem_stage->instruction->rd && mem_stage->instruction->controls.MemRead){
            id_stage->rs1_readdata = mem_stage->mem_read_data;
        }
    }
    else if(if_stage->instruction->opcode == 0x23){
        // sw x4 0(x1)
        //    rs2  rs1
        // lw followed by sw
        if(if_stage->instruction->rs1 == ex_stage->instruction->rd && (ex_stage->instruction->controls.MemRead)){//add and addi type instructions
            id_stage->num_stall = 1;
            if_stage->is_first_stalled = true;
        }
        else if(if_stage->instruction->rs2 == ex_stage->instruction->rd && (ex_stage->instruction->controls.MemRead)){//add and addi type instructions
            //debug mem to mem forwarding
        }
        // lw followed by noOp followed by sw
        else if(if_stage->instruction->rs1 == mem_stage->instruction->rd && (mem_stage->instruction->controls.MemRead)){
            id_stage->num_stall = 1;
            if_stage->is_first_stalled = true;
        }
        else if(if_stage->instruction->rs2 == mem_stage->instruction->rd && (mem_stage->instruction->controls.MemRead)){
            id_stage->rs2_readdata = mem_stage->mem_read_data;
        }
        else if(if_stage->instruction->rs1 == mem_stage->instruction->rd && (mem_stage->instruction->controls.MemRead)){
            id_stage->rs1_readdata = mem_stage->mem_read_data;
        }
        // add followed by sw
        else if(if_stage->instruction->rs1 == ex_stage->instruction->rd && (ex_stage->instruction->type == Instruction_type::R_TYPE || ex_stage->instruction->opcode == 0x13)){
            id_stage->rs1_readdata = ex_stage->alu_output;
        }
        else if(if_stage->instruction->rs2 == ex_stage->instruction->rd && (ex_stage->instruction->type == Instruction_type::R_TYPE || ex_stage->instruction->opcode == 0x13)){
            id_stage->rs2_readdata = ex_stage->alu_output;
        }
        // add followed by noOp followed by sw
        else if(if_stage->instruction->rs1 == mem_stage->instruction->rd && (mem_stage->instruction->type == Instruction_type::R_TYPE || mem_stage->instruction->opcode == 0x13)){
            id_stage->rs1_readdata = mem_stage->alu_output;
        }
        else if(if_stage->instruction->rs2 == mem_stage->instruction->rd && (mem_stage->instruction->type == Instruction_type::R_TYPE || mem_stage->instruction->opcode == 0x13)){
            id_stage->rs2_readdata = mem_stage->alu_output;
        }
    }
    else if(if_stage->instruction->opcode == 0x63){ //conditional branches
        //lw followed by beq
        if(ex_stage->instruction->controls.MemRead){
            if(if_stage->instruction->rs1 == ex_stage->instruction->rd){
                id_stage->num_stall = 2;
                if_stage->is_first_stalled = true;
            }
            if(if_stage->instruction->rs2 == ex_stage->instruction->rd){
                id_stage->num_stall = 2;
                if_stage->is_first_stalled = true;
            }
        }
        //add followed by beq
        else if(ex_stage->instruction->controls.RegWrite){
            if(if_stage->instruction->rs1 == ex_stage->instruction->rd){
                id_stage->num_stall = 1;
                if_stage->is_first_stalled = true;
            }
            if(if_stage->instruction->rs2 == ex_stage->instruction->rd){
                id_stage->num_stall = 1;
                if_stage->is_first_stalled = true;
            }
        }
        //add followed by noOp by beq
        else if(mem_stage->instruction->controls.RegWrite){
            //debug
        }
        //lw followed by NoOp by beq
        else if(mem_stage->instruction->controls.MemRead){
            //debug
        }

    }
    else if(if_stage->instruction->opcode == 0x37){ //lui
        //debug
    }
}