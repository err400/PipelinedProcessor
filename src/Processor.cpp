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
    id_latch.is_stall = false;
    //read data from if_latch
    //decode instruction
    decodeInstruction(if_latch.instruction);
    //check for data hazards
    if(checkDataHazard(if_latch.instruction, ex_latch.instruction)){
        if_latch.is_stall = id_latch.is_stall = ex_latch.is_stall = mem_latch.is_stall = wb_latch.is_stall = true;
        if_latch.num_stall = id_latch.num_stall = ex_latch.num_stall = mem_latch.num_stall = wb_latch.num_stall = 2;
        return;

    }
    if(checkDataHazard(if_latch.instruction, mem_latch.instruction)){
        if_latch.is_stall = id_latch.is_stall = ex_latch.is_stall = mem_latch.is_stall = wb_latch.is_stall = true;
        if_latch.num_stall = id_latch.num_stall = ex_latch.num_stall = mem_latch.num_stall = wb_latch.num_stall = 1;
        return;
    }
    //resolve branch
    if(if_latch.instruction.controls.is_branch){
        if(resolveBranch(if_latch.instruction)){
            if_latch.is_stall = id_latch.is_stall = ex_latch.is_stall = mem_latch.is_stall = wb_latch.is_stall = true;
            if_latch.num_stall = id_latch.num_stall = ex_latch.num_stall = mem_latch.num_stall = wb_latch.num_stall = 2;
            return;
        }
    }
    //store data in id_latch
    ex_latch.pc=id_latch.pc;
    ex_latch.instruction=id_latch.instruction;
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

void Processor::mem() { //debug
    if(mem_latch.is_stall){
        mem_latch.num_stall--;
        return;
    }
    mem_latch.is_stall = false;
    //read data from ex_latch
    //read or write based on control signal from Memory
    if(mem_latch.instruction.controls.MemRead){
        mem_latch.mem_read_data=memory.readMemory(ex_latch.alu_output);
    }
    else if(mem_latch.instruction.controls.MemWrite){
        memory.writeMemory(ex_latch.alu_output, ex_latch.rs2_value);
    }
    //store data in mem_latch
    wb_latch.pc=mem_latch.pc;
    wb_latch.instruction=mem_latch.instruction;
    //update vector of strings with MEM
}

void Processor::writeback() {
    if(wb_latch.is_stall){
        wb_latch.num_stall--;
        return;
    }
    wb_latch.is_stall = false;
    //read data from mem_latch

    //write data to register file
    if(wb_latch.instruction.controls.RegWrite){
        if(wb_latch.instruction.controls.MemtoReg){
            registers.writeRegister(mem_latch.instruction.rd, mem_latch.mem_read_data);
        }
        else if(wb_latch.instruction.controls.is_jump){
            registers.writeRegister(mem_latch.instruction.rd, mem_latch.pc+4); //jal jalr
        }
        else{
            registers.writeRegister(mem_latch.instruction.rd, mem_latch.alu_output);
        }
    }

    //store data in wb_latch

    //update vector of strings with WB
}
