#ifndef PIPELINESTAGES_HPP
#define PIPELINESTAGES_HPP

#include <stdint.h>
#include "InstructionFile.hpp"

struct IFStageData {
    uint32_t pc;
    Instruction instruction;
    int num_stall;
    bool is_stall; //The is_stall flag indicates whether the data in the pipeline register is meaningful or if it's a "bubble"
};

struct IDStageData {
    Instruction instruction; //Different instruction in each stage
    //Instruction passed for control signals
    uint32_t pc;

    uint32_t rs1_readdata;
    uint32_t rs2_readdata;
    //Immediate, rs1, rs2 value in Instruction itself
    bool branch_predicted; //branches decided after ID stage
    uint32_t branch_target;
    int num_stall;
    bool is_stall;
    //Write_data in write back stage
};

struct EXStageData {
    Instruction instruction; //Different instruction in each stage
    uint32_t pc;
    uint32_t zero;
    uint32_t alu_output; // debug
    uint32_t rs2_value; // Value to write in memory if applicable
    bool is_jump;
    int num_stall;
    bool is_stall;
};

struct MEMStageData {
    Instruction instruction; //Different instruction in each stage
    uint32_t pc;
    uint32_t mem_read_data;
    int num_stall;
    bool is_stall;
};

struct WBStageData {
    Instruction instruction; //Different instruction in each stage
    uint32_t pc;
    uint32_t write_data;
    uint8_t write_register;
    int num_stall;
    bool is_stall;
};

#endif // PIPELINESTAGES_HPP