#ifndef BASIC_PROCESSOR_HPP
#define BASIC_PROCESSOR_HPP

#include "Registers.hpp"
#include "Memory.hpp"
#include "PipelineStages.hpp"
#include "Utils.hpp"

class Basic_Processor {
    protected:
        IFStageData if_latch;
        IDStageData id_latch;
        EXStageData ex_latch;
        MEMStageData mem_latch;
        WBStageData wb_latch;

        Registers registers;
        Memory memory;

        uint32_t pc = 0;

        int cycles;
        bool is_completed;
        protected:
        virtual void cycle() = 0;

        virtual void fetch() = 0;
        virtual void decode() = 0;
        virtual void execute() = 0;
        virtual void mem() = 0;
        virtual void writeback() = 0;
    
    public:
        // Basic_Processor(); // debug
        void run();
        void loadProgram(const char* filename);

};

#endif