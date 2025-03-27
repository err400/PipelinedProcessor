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
    bool resolveBranch(Instruction instr);
};


#endif // PROCESSOR_HPP