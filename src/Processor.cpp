#include "Headers/Processor.hpp"


void Processor::cycle() {
    writeback();
    mem();
    execute();
    decode();
    fetch();
}

void Processor::fetch() {
    if(if_latch.num_stall > 0){
        if_latch.num_stall--;
        return;
    }
    else{
        if_latch.is_stall = false;
        if(cycles == 0){
            if_latch.pc = 0;
        }
        else{
            if_latch.pc = pc;
            // read instructions from IM
            Instruction new_instr;
            new_instr.raw = getInstruction(pc);
            id_latch.instruction = new_instr;
            id_latch.pc = if_latch.pc;
            pc += 4; // increment the global pc for next instruction // debug
            // jal -> update global pc in that case
        }
    }
    //pc
    //read instruction from instruction memory
    //Update space in vector of strings according to cycle (Append a space)
    //increment pc
    //store data in if_latch

    //update vector of strings with IF
}

void Processor::decode() {
    if(id_latch.num_stall > 0){
        id_latch.num_stall--;
        return;
    }
    else{
        //read data from if_latch
        //decode instruction
        //check for data hazards
        if(checkDataHazard(if_latch.instruction, ex_latch.instruction)){
            if_latch.is_stall = true; //debug
            if_latch.num_stall = 2;
            id_latch.is_stall = true;
            id_latch.num_stall = 2;
            ex_latch.is_stall = true;
            ex_latch.num_stall = 2;
            mem_latch.is_stall = true;
            mem_latch.num_stall = 2;
            wb_latch.is_stall = true;
            wb_latch.num_stall = 2;

        }
        else if(checkDataHazard(if_latch.instruction, mem_latch.instruction)){
            if_latch.is_stall = id_latch.is_stall = ex_latch.is_stall = mem_latch.is_stall = wb_latch.is_stall = true;
            if_latch.num_stall = id_latch.num_stall = ex_latch.num_stall = mem_latch.num_stall = wb_latch.num_stall = 1;
        }
        
    }
    //resolve branch

    //store data in id_latch
    //3 cases checkhazard(if_latch ins, ex_latch ins) // // update stalls
    //update vector of strings with ID
}

void Processor::execute() {
    if(ex_latch.num_stall > 0){
        ex_latch.num_stall--;
        return;
    }
    else{
        // add more alus // debug
        // alu output in the case of jal / jalr
        // negative imm // debug
        ex_latch.is_stall = false;
        if(ex_latch.instruction.controls.is_jump){
            pc += imm;
            ex_latch.is_jump = true;
        }
        ex_latch.alu_output = 0;
        // debug
        mem_latch.instruction = ex_latch.instruction;
        mem_latch.pc = ex_latch.pc;
        mem_latch.alu_output = ex_latch.alu_output;
    }
    //read data from id_latch
    //execute instruction
    //store data in ex_latch

    //update vector of strings with EX
}

void Processor::mem() {
    if(mem_latch.is_stall){
        mem_latch.num_stall--;
        return;
    }
    else{

    }
    //read data from ex_latch
    //read or write based on control signal from Memory
    //store data in mem_latch

    //update vector of strings with MEM
}

void Processor::writeback() {
    if(wb_latch.is_stall){
        wb_latch.num_stall--;
        return;
    }
    else{

    }
    //read data from mem_latch

    //write data to register file
    //store data in wb_latch

    //update vector of strings with WB
}
