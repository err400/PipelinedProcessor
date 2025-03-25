#ifndef PIPELINESTAGES_HPP
#define PIPELINESTAGES_HPP

#include <stdint.h>
#include "InstructionFile.hpp"

struct IFStageData {
    uint32_t pc;
    bool is_first_stalled; // since id and ex will always be executed together
    Instruction* instruction;
    int num_stall;
    bool is_stall; //The is_stall flag indicates whether the data in the pipeline register is meaningful or if it's a "bubble"
    bool valid; // instruction inside the stage is not null
};

struct IDStageData {
    Instruction* instruction; //Different instruction in each stage
    //Instruction passed for control signals
    uint32_t pc;
    bool branch_is_taken_resolved;
    uint32_t rs1_readdata;
    uint32_t rs2_readdata;
    //Immediate, rs1, rs2 value in Instruction itself
    bool branch_predicted; //branches decided after ID stage
    uint32_t branch_target;
    int num_stall;
    bool is_stall;
    bool valid;
    //Write_data in write back stage
};

struct EXStageData {
    Instruction* instruction; //Different instruction in each stage
    uint32_t pc;
    uint32_t zero;
    int32_t alu_output; // debug
    uint32_t rs2_value; // Value to write in memory if applicable
    bool is_jump;
    int num_stall;
    bool is_stall;
    bool valid;
};

// sw x4 0(x1)
struct MEMStageData {
    Instruction* instruction; //Different instruction in each stage
    uint32_t pc;
    uint32_t mem_read_data;
    int32_t alu_output; // can be the address where to write
    int num_stall;
    bool is_stall;
    bool valid;
};

struct WBStageData {
    Instruction* instruction; //Different instruction in each stage
    uint32_t pc;
    uint32_t write_data;
    uint8_t write_register;
    int num_stall;
    bool is_stall;
    bool valid;
};

#endif // PIPELINESTAGES_HPP