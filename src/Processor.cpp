#include "Headers/Processor.hpp"

bool Processor::resolveBranch(Instruction instr){
    // read reg values
    int32_t rs1_data = registers.readRegister(instr.rs1);
    int32_t rs2_data = registers.readRegister(instr.rs2);
    bool branch_taken = false;
    switch(instr.func3){
        case 0x0: // beq
            branch_taken = (rs1_data == rs2_data);
            break;
        case 0x1: // bne
            branch_taken = (rs1_data != rs2_data);
            break;
        case 0x4: // blt
            branch_taken = (rs1_data < rs2_data);
            break;
        case 0x5: // bge
            branch_taken = (rs1_data >= rs2_data);
            break;
        case 0x6: // bltu
            branch_taken = (rs1_data < rs2_data);
            break;
        case 0x7: // bgeu
            branch_taken = (rs1_data >= rs2_data);
            break;
    }
    if(branch_taken){
        pc = if_latch.pc + instr.imm; // change the global pc for the IF stage
    }
    return branch_taken;
    // update pc if flag is true
}
void Processor::cycle() {
    // default value in the vector of strings for each instruction
    for(int i = 0;i<instructionMemory.size();i++){
        instructionMemory[i].vec.push_back(" ");
    }
    printf("In cycle %d\n", cycles); // debug
    writeback();
    mem();
    execute();
    decode();
    fetch();
}

void Processor::fetch() {
    printf("fetch in processor\n"); // debug
    if(if_latch.num_stall > 0){
        // push - in the instruction vector of strings
        
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
            // jal -> update global pc in that case
        }
        Instruction new_instr;
        new_instr = getInstruction(if_latch.pc);
        if_latch.valid = true;
        id_latch.valid = true;
        if_latch.instruction = new_instr;
        pc += 4; // increment the global pc for next instruction // debug
    }

    if(if_latch.is_first_stalled){
        if_latch.num_stall = id_latch.num_stall;
        if_latch.is_stall = true;
        if_latch.is_first_stalled = false;
    }
    
    //pc
    //read instruction from instruction memory
    //Update space in vector of strings according to cycle (Append a space)
    //increment pc
    //store data in if_latch

    //update vector of strings with IF
}

void Processor::decode() {
    printf("decode in processor\n"); // debug
    if(id_latch.num_stall > 0){
        id_latch.num_stall--;
        if(id_latch.num_stall == 0){
            id_latch.is_stall = false;
        }
        return;
    }
    if(!id_latch.valid){
        return;
    }
    // comes here when there is no stall or first time stall is to be detected
    id_latch.is_stall = false;
    //read data from if_latch
    //decode instruction
    decodeInstruction(if_latch.instruction);
    //check for data hazards
    if(checkDataHazard(if_latch.instruction, ex_latch.instruction)){
        printf("Data hazard1 detected\n"); // debug
        printf("cycle: %d\n", cycles); // debug
        // propogate the stalls
        if_latch.is_first_stalled = true;
        id_latch.is_stall = true;
        id_latch.num_stall = 2;
    }
    if(checkDataHazard(if_latch.instruction, mem_latch.instruction)){
        printf("Data hazard2 detected\n"); // debug
        printf("cycle: %d\n", cycles); // debug
        // propogate the stalls
        if_latch.is_first_stalled = true;
        id_latch.is_stall = true;
        id_latch.num_stall = 1;
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
    id_latch.instruction = if_latch.instruction;
    id_latch.pc = if_latch.pc;
    id_latch.valid = true;
    ex_latch.valid = true;
    //update vector of strings with ID
}

void Processor::execute() {
    printf("execute in processor\n"); // debug

    if(id_latch.is_stall){
        ex_latch.is_stall = true;
        ex_latch.num_stall = id_latch.num_stall;
    }

    if(ex_latch.num_stall > 0){
        ex_latch.num_stall--;
        return;
    }
    else{
        if(!ex_latch.valid){
            return;
        }
        // alu output in the case of jal / jalr
        // negative imm // debug
        ex_latch.is_stall = false;
        int32_t op1 = registers.readRegister(id_latch.instruction.rs1);
        int32_t op2;

        if(id_latch.instruction.controls.AluSrc){
            op2 = id_latch.instruction.imm;
        }
        else{
            op2 = registers.readRegister(id_latch.instruction.rs2);
        }

        

        switch(id_latch.instruction.controls.AluOp){
            case ALUOp::ADD:
                ex_latch.alu_output = op1 + op2;
                break;
            case ALUOp::SUB:
                ex_latch.alu_output = op1 - op2;
                break;
            case ALUOp::SLL:
                ex_latch.alu_output = op1 << op2;
                break;
            case ALUOp::SLT:
                ex_latch.alu_output = ((int32_t)op1 < (int32_t)op2) ? 1 : 0;
                break;
            case ALUOp::SLTU:
                ex_latch.alu_output = (op1 < op2) ? 1 : 0;
                break;
            case ALUOp::XOR:
                ex_latch.alu_output = op1 ^ op2;
                break;
            case ALUOp::SRL:
                ex_latch.alu_output = op1 >> op2;
                break;
            case ALUOp::SRA:
                ex_latch.alu_output = (int32_t)op1 >> op2;
                break;
            case ALUOp::OR:
                ex_latch.alu_output = op1 | op2;
                break;
            case ALUOp::AND:
                ex_latch.alu_output = op1 & op2;
                break;
            case ALUOp::MUL:
                ex_latch.alu_output = op1 * op2;
                break;
            case ALUOp::DIV:
                // check for division by zero
                if(op2 == 0){
                    // std::cerr << "Error: Division by zero" << std::endl;
                    ex_latch.alu_output = 0;
                }
                else{
                    ex_latch.alu_output = op1 / op2;
                }
                break;
            case ALUOp::REM:
                if(op2 == 0){
                    ex_latch.alu_output = 0;
                }
                else{
                    ex_latch.alu_output = op1 % op2;
                }
                break;
        }
        
        if(id_latch.instruction.controls.is_jump){
            if(id_latch.instruction.opcode == 0x6F) { 
                // jal
                pc = ex_latch.pc + id_latch.instruction.imm;
            } 
            else if(id_latch.instruction.opcode == 0x67) { 
                // jalr
                // to make sure the number is even
                pc = (op1 + id_latch.instruction.imm) & ~1;
            }
            ex_latch.is_jump = true;
            ex_latch.is_jump = true;
        }
        // branches resolved in ID stage
        // set alu_output to register read data in case of sw instruction
        ex_latch.pc=id_latch.pc;
        ex_latch.instruction=id_latch.instruction;
        ex_latch.rs2_value = id_latch.instruction.rs2;
        mem_latch.valid = true;
    }
    //read data from id_latch
    //execute instruction
    //store data in ex_latch

    //update vector of strings with EX
}

void Processor::mem() { //debug
    printf("mem in processor\n"); // debug
    if(ex_latch.is_stall){
        mem_latch.is_stall = true;
        mem_latch.num_stall = ex_latch.num_stall + 1;
    }


    if(mem_latch.num_stall > 0){
        mem_latch.num_stall--;
        return;
    }
    if(!mem_latch.valid){
        return;
    }
    mem_latch.is_stall = false;
    //read data from ex_latch
    //read or write based on control signal from Memory
    // printf("mem_latch.instruction: %s\n", mem_latch.instruction.instStr.c_str()); // debug
    if(ex_latch.instruction.controls.MemRead){
        // printf("debug1 : memread in processor\n"); // debug
        mem_latch.mem_read_data=memory.readMemory(ex_latch.alu_output);
       
    }
    else if(ex_latch.instruction.controls.MemWrite){
        // printf("debug1 : memwrite in processor\n"); // debug
        memory.writeMemory(ex_latch.alu_output, ex_latch.rs2_value);
    }
    //store data in mem_latch
    mem_latch.instruction = ex_latch.instruction;
    mem_latch.pc = ex_latch.pc;
    mem_latch.alu_output = ex_latch.alu_output;
    //update vector of strings with MEM
    wb_latch.valid = true;
}

void Processor::writeback() {
    printf("writeback in processor\n"); // debug

    if(mem_latch.is_stall){
        wb_latch.is_stall = true;
        wb_latch.num_stall = mem_latch.num_stall + 1;
    }

    if(wb_latch.num_stall > 0){
        wb_latch.num_stall--;
        return;
    }
    if(!wb_latch.valid){
        // no instr currently in writeback stage (not even in stall)
        return;
    }
    wb_latch.is_stall = false;
    //read data from mem_latch

    //write data to register file
    if(mem_latch.instruction.controls.RegWrite){
        // printf("debug1 : regwrite in processor\n"); // debug
        if(mem_latch.instruction.controls.MemtoReg){
            registers.writeRegister(mem_latch.instruction.rd, mem_latch.mem_read_data);
        }
        else if(mem_latch.instruction.controls.is_jump){
            registers.writeRegister(mem_latch.instruction.rd, mem_latch.pc+4); //jal jalr
        }
        else{
            registers.writeRegister(mem_latch.instruction.rd, mem_latch.alu_output);
        }
    }

    //store data in wb_latch
    wb_latch.pc=mem_latch.pc;
    wb_latch.instruction=mem_latch.instruction;
    wb_latch.write_data = mem_latch.alu_output; // debug
    //update vector of strings with WB
}

Processor::Processor() {
    cycles = 0;
    pc = 0;
    if_latch.is_first_stalled = false;
    if_latch.valid = id_latch.valid = ex_latch.valid = mem_latch.valid = wb_latch.valid = false;
    if_latch.is_stall = false;
    id_latch.is_stall = ex_latch.is_stall = mem_latch.is_stall = wb_latch.is_stall = false;
    if_latch.num_stall = id_latch.num_stall = ex_latch.num_stall = mem_latch.num_stall = wb_latch.num_stall = 0;
}