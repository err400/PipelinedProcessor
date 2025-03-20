#ifndef PIPELINESTAGES_HPP
#define PIPELINESTAGES_HPP

#include <stdint.h>
#include "InstructionFile.h"

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



#endif // PIPELINESTAGES_HPP