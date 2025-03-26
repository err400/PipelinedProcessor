#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "Basic_Processor.hpp"

class Processor : public Basic_Processor {
    private:
    void cycle() override;

    void fetch() override;
    void decode() override;
    void execute() override;
    void mem() override;
    void writeback() override;

    public:
    Processor(bool is_forward);
    bool is_forwarded;
    bool resolveBranch(Instruction instr);
    void forward(IFStageData* if_stage, IDStageData* id_stage, EXStageData* ex_stage, MEMStageData* mem_stage);
};


#endif // PROCESSOR_HPP