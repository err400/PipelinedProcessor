#include "Headers/Basic_Processor.hpp"

void Basic_Processor::loadProgram(const char* filename) {
    readMachineCode(filename);
}

void Basic_Processor::run(int cyclecount) {
    while (true) {
        cycle();
        cycles++;
        if(cycles>cyclecount){
            break;
        }
    }
}