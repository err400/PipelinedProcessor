#include "Headers/Basic_Processor.hpp"

void Basic_Processor::loadProgram(const char* filename) {
    readMachineCode(filename);
}

void Basic_Processor::run() {
    while (!is_completed) {
        cycle();
        cycles++;
        if(cycles>15){
            break;  //debug
        }
    }
}