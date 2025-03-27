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
        std::cerr << "Error: Unable to open file " << fileName << std::endl;
        exit(1);
    }
    char line[60];
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
    return &(instructionMemory[address / 4]);
}

void outputStageandCycles(const string& filename, bool is_forwarded) {
    fs::path path_obj(filename);
    string base_filename = path_obj.filename().string(); 
    // string file_dest;
    // if(is_forwarded){
    //     file_dest = "../outputfiles/" + base_filename.substr(0, base_filename.find_last_of('.')) + "_forward_out.txt";
    // }
    // else{
    //     file_dest = "../outputfiles/" + base_filename.substr(0, base_filename.find_last_of('.')) + "_noforward_out.txt";
    // }
    ostream& outputFile = cout;
    // if (!outputFile.is_open()) {
    //     cerr << "Error: Unable to open file " << file_dest << endl;
    //     exit(1);
    // }
    int n = instructionMemory.size();
    for(int i = 0;i<n;i++){
        Instruction instr = instructionMemory[i];
        outputFile<<left << setw(20)<<instr.instStr<<";  ";
        for(auto it: instr.vec){
            outputFile<< setw(3)<<it<<";";
        }
        outputFile<<endl;
    }
    // outputFile.close();
}

bool checkDataHazardrs1(Instruction* instruction1, Instruction* instruction2) {
    if(instruction2->controls.MemWrite){
        return false;
    }
    // Check if instruction1 is a JAL,LUI or AUIPC
    if (instruction1->type == Instruction_type::UJ_TYPE || instruction1->type == Instruction_type::U_TYPE)
        return false; // no rs1
    if (instruction2->type == Instruction_type::S_TYPE || instruction2->type == Instruction_type::SB_TYPE)
        return false; // no rd
    int rs1 = instruction1->rs1;
    int rd = instruction2->rd;
    
    if (rs1 == rd)
        return true;
    
    return false;
}

bool checkDataHazardrs2(Instruction* instruction1, Instruction* instruction2) {
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
    if(if_stage->instruction->type == Instruction_type::R_TYPE || if_stage->instruction->opcode == 0x13){
        //check prev instruction
        if(ex_stage->instruction != nullptr){
            if(if_stage->instruction->rs1 == ex_stage->instruction->rd){
                if(ex_stage->instruction->controls.MemRead){
                    id_stage->num_stall = 1;
                    if_stage->is_first_stalled = true;
                    id_stage->is_stall = true;
                    return;
                } //ex gets data from mem stage (lw followed by add)
                // lw x5 0(x1)
                // add x6 x5 x4
                else{
                    id_stage->rs1_readdata = ex_stage->alu_output;
                    return;
                } //ex gets data from ex stage (add followed by add)
                // add x5 x4 x3
                // add x6 x5 x4
            }
        }
        if(mem_stage->instruction != nullptr){
            if(if_stage->instruction->rs1 == mem_stage->instruction->rd){
                if(mem_stage->instruction->controls.MemRead){
                    id_stage->rs1_readdata = mem_stage->mem_read_data;
                    return;
                } //ex gets data from mem stage (lw followed by NoOp followed by add)
                else{
                    id_stage->rs1_readdata = mem_stage->alu_output; //ex gets data from mem stage (add followed by NoOp followed by add)
                    return;
                }
            }
        }
    }
    else if(if_stage->instruction->type == Instruction_type::R_TYPE){ // pther than immediate types
        if(ex_stage->instruction != nullptr){
            if(if_stage->instruction->rs2 == ex_stage->instruction->rd){
                if(ex_stage->instruction->controls.MemRead){
                    id_stage->num_stall = 1;
                    if_stage->is_first_stalled = true;
                    id_stage->is_stall = true;
                    return;
                } //ex gets data from mem stage (lw followed by add)
                else{
                    id_stage->rs2_readdata = ex_stage->alu_output;
                    return;
                } //ex gets data from ex stage (add followed by add)
            }
        }
        if(mem_stage->instruction != nullptr){
            if(if_stage->instruction->rs2 == mem_stage->instruction->rd){
                if(mem_stage->instruction->controls.MemRead){
                    id_stage->rs2_readdata = mem_stage->mem_read_data;
                    return;
                } //ex gets data from mem stage (lw followed by NoOp followed by add)
                else{
                    id_stage->rs2_readdata = mem_stage->alu_output; //ex gets data from mem stage (add followed by NoOp followed by add)
                    return;
                }
            }
        }
    }
    else if(if_stage->instruction->opcode == 0x03){ //lw
        if(ex_stage->instruction != nullptr){
            // add followed by lw
            if(if_stage->instruction->rs1 == ex_stage->instruction->rd && (ex_stage->instruction->type == Instruction_type::R_TYPE || ex_stage->instruction->opcode == 0x13)){//add and addi type instructions
                id_stage->rs1_readdata = ex_stage->alu_output; //case of address of lw dependent on output
                return;
            }
            // lw followed by lw
            else if(if_stage->instruction->rs1 == ex_stage->instruction->rd && ex_stage->instruction->controls.MemRead){
                id_stage->num_stall = 1;
                if_stage->is_first_stalled = true;
                id_stage->is_stall = true;
                return;
            }
        }
        if(mem_stage->instruction != nullptr){
            // add followed by noOp followed by lw
            if(if_stage->instruction->rs1 == mem_stage->instruction->rd && (mem_stage->instruction->type == Instruction_type::R_TYPE || mem_stage->instruction->opcode == 0x13)){
                id_stage->rs1_readdata = mem_stage->alu_output; //case of address of lw dependent on output
                return;
            }
            // lw followed by noOp followed by lw
            else if(if_stage->instruction->rs1 == mem_stage->instruction->rd && mem_stage->instruction->controls.MemRead){
                id_stage->rs1_readdata = mem_stage->mem_read_data;
                return;
            }
        }
    }
    else if(if_stage->instruction->opcode == 0x23){ //sw
        // sw x4 0(x1)
        //    rs2  rs1
        // lw followed by sw
        if(ex_stage->instruction != nullptr){
            if((if_stage->instruction->rs2 == ex_stage->instruction->rd) && (if_stage->instruction->rs1== ex_stage->instruction->rd) && (ex_stage->instruction->controls.MemRead)){
                id_stage->num_stall = 2;
                if_stage->is_first_stalled = true;
                id_stage->is_stall = true;
                return;
            }
            else if(if_stage->instruction->rs1 == ex_stage->instruction->rd && (ex_stage->instruction->controls.MemRead)){//add and addi type instructions
                id_stage->num_stall = 1;
                if_stage->is_first_stalled = true;
                id_stage->is_stall = true;
                return;
            }
            else if(if_stage->instruction->rs2 == ex_stage->instruction->rd && (ex_stage->instruction->controls.MemRead)){//add and addi type instructions
                //continue mem to mem forwarding
                return;
            }
            // add followed by sw
            else if(if_stage->instruction->rs1 == ex_stage->instruction->rd && (ex_stage->instruction->type == Instruction_type::R_TYPE || ex_stage->instruction->opcode == 0x13)){
                id_stage->rs1_readdata = ex_stage->alu_output;
                return;
            }
            else if(if_stage->instruction->rs2 == ex_stage->instruction->rd && (ex_stage->instruction->type == Instruction_type::R_TYPE || ex_stage->instruction->opcode == 0x13)){
                id_stage->rs2_readdata = ex_stage->alu_output;
                return;
            }
        }
        if(mem_stage->instruction != nullptr){
            // lw followed by noOp followed by sw
            if(if_stage->instruction->rs1 == mem_stage->instruction->rd && (mem_stage->instruction->controls.MemRead)){
                id_stage->instruction->rs1 = mem_stage->mem_read_data;
            }
            else if(if_stage->instruction->rs2 == mem_stage->instruction->rd && (mem_stage->instruction->controls.MemRead)){
                id_stage->rs2_readdata = mem_stage->mem_read_data;
            }
            else if(if_stage->instruction->rs1 == mem_stage->instruction->rd && (mem_stage->instruction->controls.MemRead)){
                id_stage->rs1_readdata = mem_stage->mem_read_data;
            }
            // add followed by noOp followed by sw
            else if(if_stage->instruction->rs1 == mem_stage->instruction->rd && (mem_stage->instruction->type == Instruction_type::R_TYPE || mem_stage->instruction->opcode == 0x13)){
                id_stage->rs1_readdata = mem_stage->alu_output;
            }
            else if(if_stage->instruction->rs2 == mem_stage->instruction->rd && (mem_stage->instruction->type == Instruction_type::R_TYPE || mem_stage->instruction->opcode == 0x13)){
                id_stage->rs2_readdata = mem_stage->alu_output;
            }
            return;
        }
    }
    else if(if_stage->instruction->opcode == 0x63){ //conditional branches
        if(ex_stage->instruction != nullptr){
            //lw followed by beq
            if(ex_stage->instruction->controls.MemRead){
                if(if_stage->instruction->rs1 == ex_stage->instruction->rd){
                    id_stage->num_stall = 2;
                    if_stage->is_first_stalled = true;
                    id_stage->is_stall = true;
                }
                if(if_stage->instruction->rs2 == ex_stage->instruction->rd){
                    id_stage->num_stall = 2;
                    if_stage->is_first_stalled = true;
                    id_stage->is_stall = true;
                }
            }
            //add followed by beq
            else if(ex_stage->instruction->controls.RegWrite){
                if(if_stage->instruction->rs1 == ex_stage->instruction->rd){
                    id_stage->num_stall = 1;
                    if_stage->is_first_stalled = true;
                    id_stage->is_stall = true;
                }
                if(if_stage->instruction->rs2 == ex_stage->instruction->rd){
                    id_stage->num_stall = 1;
                    if_stage->is_first_stalled = true;
                    id_stage->is_stall = true;
                }
            }
            return;
        }
        if(mem_stage->instruction != nullptr){
            //add followed by noOp by beq
            if(mem_stage->instruction->controls.RegWrite){
                //continue
            }
            //lw followed by NoOp by beq
            else if(mem_stage->instruction->controls.MemRead){
                if(if_stage->instruction->rs1 == mem_stage->instruction->rd){
                    id_stage->num_stall = 1;
                    if_stage->is_first_stalled = true;
                    id_stage->is_stall = true;
                }
                if(if_stage->instruction->rs2 == mem_stage->instruction->rd){
                    id_stage->num_stall = 1;
                    if_stage->is_first_stalled = true;
                    id_stage->is_stall = true;
                }
            }
            return;
        }

    }
    else if(if_stage->instruction->opcode == 0x37){ //lui
        //debug
    }
    else if(if_stage->instruction->opcode == 0x67){ //jalr
        if(ex_stage->instruction != nullptr){
            //add followed by jalr
            if(if_stage->instruction->type == Instruction_type::R_TYPE || if_stage->instruction->opcode == 0x13){
                if(if_stage->instruction->rs1 == ex_stage->instruction->rd){
                    id_stage->num_stall = 1;
                    if_stage->is_first_stalled = true;
                    id_stage->is_stall = true;
                }
            }
            //add noOp followed by jalr
            //continue

            //lw followed by jalr
            else if(ex_stage->instruction->controls.MemRead){
                if(if_stage->instruction->rs1 == ex_stage->instruction->rd){
                    id_stage->num_stall = 3;
                    if_stage->is_first_stalled = true;
                    id_stage->is_stall = true;
                }
            }
        }
        if(mem_stage->instruction != nullptr){
            //lw noOp jalr
            if(mem_stage->instruction->controls.MemRead){
                if(if_stage->instruction->rs1 == mem_stage->instruction->rd){
                    id_stage->num_stall = 2;
                    if_stage->is_first_stalled = true;
                    id_stage->is_stall = true;
                }
            }
        }

    }
}



void forward_Dataflow(IFStageData* if_latch, IDStageData* id_latch, EXStageData* ex_latch, MEMStageData* mem_latch, WBStageData* wb_latch, Registers* registers){
    //lw followed by add
    if(id_latch->instruction->type == Instruction_type::R_TYPE || id_latch->instruction->opcode == 0x13){
        if(id_latch->instruction->rs1 == mem_latch->instruction->rd){
            id_latch->rs1_readdata = mem_latch->alu_output;
        }
        else if(id_latch->instruction->rs2 == mem_latch->instruction->rd){
            id_latch->rs2_readdata = mem_latch->alu_output;
        }
    }
    //lw followed by lw
    else if(id_latch->instruction->opcode == 0x03){
        if(id_latch->instruction->rs1 == mem_latch->instruction->rd){
            id_latch->rs1_readdata = mem_latch->mem_read_data;
        }
    }
    //lw followed by sw
    else if(id_latch->instruction->opcode == 0x23){
        //lw x4 0(x1)
        //sw x4 0(x4)
        if((id_latch->instruction->rs2 == wb_latch->instruction->rd) && (id_latch->instruction->rs1== wb_latch->instruction->rd) && (wb_latch->instruction->controls.MemRead)){
            id_latch->rs1_readdata = registers->readRegister(if_latch->instruction->rs1);
            id_latch->rs2_readdata = registers->readRegister(if_latch->instruction->rs2);
        }
        //lw x4 0(x1)
        //sw x2 0(x4)
        else if(id_latch->instruction->rs1 == mem_latch->instruction->rd && (mem_latch->instruction->controls.MemRead)){//add and addi type instructions
            id_latch->rs1_readdata = mem_latch->mem_read_data;
        }
        //lw x4 0(x1)
        //sw x4 0(x2)
        else if(ex_latch->instruction->rs2 == wb_latch->instruction->rd && (wb_latch->instruction->controls.MemRead)){
            ex_latch->alu_output = wb_latch->write_data;
        }
    }
    else if(id_latch->instruction->opcode == 0x63){ //conditionals
        //lw followed by beq
        //lw followed by NoOp by beq
        //lw x4 0(x2)
        //beq x4 x0 8
        if(wb_latch->instruction->controls.MemRead){
            if(id_latch->instruction->rs1 == wb_latch->instruction->rd){
                id_latch->rs1_readdata = registers->readRegister(id_latch->instruction->rs1);
            }
            if(id_latch->instruction->rs2 == wb_latch->instruction->rd){
                id_latch->rs2_readdata = registers->readRegister(id_latch->instruction->rs2);
            }
        }
        //add followed by beq
        //add followed by noOp by beq
        else if((mem_latch->instruction->type == Instruction_type::R_TYPE || mem_latch->instruction->opcode == 0x13)){
            if(id_latch->instruction->rs1 == mem_latch->instruction->rd){
                id_latch->rs1_readdata = mem_latch->alu_output;
            }
            if(id_latch->instruction->rs2 == mem_latch->instruction->rd){
                id_latch->rs2_readdata = mem_latch->alu_output;
            }
        }
    }
    else if(id_latch->instruction->opcode == 0x67){ //jalr
        //add followed by jalr
        //add noOp followed by jalr
        if(id_latch->instruction->type == Instruction_type::R_TYPE || id_latch->instruction->opcode == 0x13){
            if(id_latch->instruction->rs1 == mem_latch->instruction->rd){
                id_latch->rs1_readdata = mem_latch->alu_output;
            }
        }
        //lw followed by jalr
        else if(wb_latch->instruction->controls.MemRead){
            if(id_latch->instruction->rs1 == wb_latch->instruction->rd){
                id_latch->rs1_readdata = wb_latch->write_data;
            }
        }
        //lw noOp jalr
        else if(mem_latch->instruction->controls.MemRead){
            if(id_latch->instruction->rs1 == mem_latch->instruction->rd){
                id_latch->rs1_readdata = mem_latch->mem_read_data;
            }
        }

    }
}