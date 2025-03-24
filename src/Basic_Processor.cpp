#include "Headers/Basic_Processor.hpp"

void Basic_Processor::loadProgram(const char* filename) {
    readMachineCode(filename);
}

void Basic_Processor::run() {
    while (true) {
        cycle();
        cycles++;
        if(cycles>10){
            break;  //debug
        }
    }
}