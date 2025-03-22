#include "Headers/Processor.hpp"


void Processor::cycle() {
    writeback();
    mem();
    execute();
    decode();
    fetch();
}

void Processor::fetch() {
    //pc
    //read instruction from instruction memory
    //Update space in vector of strings according to cycle (Append a space)
    //increment pc
    //store data in if_latch

    //update vector of strings with IF
}

void Processor::decode() {
    //read data from if_latch
    //decode instruction
    //check for data hazards
    //resolve branch
    //store data in id_latch

    //update vector of strings with ID
}

void Processor::execute() {
    //read data from id_latch
    //execute instruction
    //store data in ex_latch

    //update vector of strings with EX
}

void Processor::mem() {
    //read data from ex_latch
    //read or write based on control signal from Memory
    //store data in mem_latch

    //update vector of strings with MEM
}

void Processor::writeback() {
    //read data from mem_latch
    //write data to register file
    //store data in wb_latch

    //update vector of strings with WB
}
