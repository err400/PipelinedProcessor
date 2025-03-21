#ifndef PIPELINESTAGES_HPP
#define PIPELINESTAGES_HPP

#include <stdint.h>
#include "InstructionFile.hpp"

struct IFStageData {
    uint32_t pc;
    Instruction instruction;
    bool valid; //The valid flag indicates whether the data in the pipeline register is meaningful or if it's a "bubble"
};

struct IDStageData {
    Instruction instruction; //Different instruction in each stage
    //Instruction passed for control signals
    uint32_t pc;
    uint32_t rs1_readdata;
    uint32_t rs2_readdata;
    bool branch_predicted; //branches decided after ID stage
    uint32_t branch_target;
    bool valid;
    //Write_data in write back stage
};

struct EXStageData {
    Instruction instruction; //Different instruction in each stage
    uint32_t pc;
    uint32_t zero;
    uint32_t alu_output;
    uint32_t input1;
    uint32_t input2;
    bool valid;
};

struct MEMStageData {
    Instruction instruction; //Different instruction in each stage
    uint32_t pc;
    uint32_t address;
    uint32_t write_data;
    uint32_t read_data;
    bool valid;
};

struct WBStageData {
    Instruction instruction; //Different instruction in each stage
    uint32_t pc;
    uint32_t write_data;
    uint8_t write_register;
    bool reg_write;
    bool valid;
};



#endif // PIPELINESTAGES_HPP