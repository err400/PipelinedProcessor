#include "Headers/Basic_Processor.hpp"

void Basic_Processor::loadProgram(const char* filename) {
    readMachineCode(filename);
}

void Basic_Processor::run(int cyclecount) {
    printf("Came in run\n"); // debug
    while (true) {
        cycle();
        cycles++;
        if(cycles>cyclecount){
            break;  //debug
        }
    }
}