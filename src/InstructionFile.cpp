#include "InstructionFile.hpp"
#include <iostream>

//decode function

void decode(Instruction& instr){
    // first 7 bits : opcode 
    instr.opcode = instr.raw & 0x7F;
    instr.rd = (instr.raw >> 7) & 0x1F;
    instr.func3 = (instr.raw >> 12) & 0x7;
    instr.rs1 = (instr.raw >> 15) & 0x1F;
    instr.rs2 = (instr.raw >> 20) & 0x1F;
    instr.func7 = (instr.raw >> 25) & 0x7F;
    // construct the value of imm according to the type of instruction
    if(instr.opcode == 0x33){
        instr.type = Instruction_type::R_TYPE;
        instr.imm = 0;
    }
    else if(instr.opcode == 0x03 || instr.opcode == 0x13 || instr.opcode == 0x67 || instr.opcode == 0x73){
        instr.type = Instruction_type::I_TYPE;
        instr.imm = (int32_t)((instr.raw) >> 20);
    }
    else if(instr.opcode == 0x23){
        instr.type = Instruction_type::S_TYPE;
        uint32_t imm11_5 = (instr.raw >> 25) & 0x7F;  // bits 31:25
        uint32_t imm4_0 = (instr.raw >> 7) & 0x1F;    // bits 11:7
        instr.imm = (int32_t)((imm11_5 << 5) | imm4_0);
    }
    else if(instr.opcode == 0x63){
        instr.type = Instruction_type::SB_TYPE;
        uint32_t imm12 = (instr.raw >> 31) & 0x1;    // bit 31
        uint32_t imm11 = (instr.raw >> 7) & 0x1;     // bit 7
        uint32_t imm10_5 = (instr.raw >> 25) & 0x3F; // bits 30:25
        uint32_t imm4_1 = (instr.raw >> 8) & 0xF;    // bits 11:8

        instr.imm = (int32_t)((imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1));
    }
    else if(instr.opcode == 0x6F){
        instr.type = Instruction_type::UJ_TYPE;
        uint32_t imm20 = (instr.raw >> 31) & 0x1;     // bit 31 (sign bit)
        uint32_t imm10_1 = (instr.raw >> 21) & 0x3FF; // bits 30:21
        uint32_t imm11 = (instr.raw >> 20) & 0x1;     // bit 20
        uint32_t imm19_12 = (instr.raw >> 12) & 0xFF; // bits 19:12
        instr.imm = (int32_t)((imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1));
        
    }
    else if(instr.opcode == 0x17 || instr.opcode == 0x37){
        instr.type = Instruction_type::U_TYPE;
        instr.imm = (int32_t)(instr.raw & 0xFFFFF000);
    }
}