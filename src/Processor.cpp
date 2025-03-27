#include "Headers/Processor.hpp"

void Processor::cycle() {
    // default value in the vector of strings for each instruction
    for(int i = 0;i<(int)instructionMemory.size();i++){
        instructionMemory[i].vec.push_back(" ");
    }
    writeback();
    mem();
    execute();
    decode();
    fetch();
}

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
    return branch_taken;
}

void Processor::fetch() {
    // make if latch invalid when the pc exceeds the instructions in the IM
    if(!if_latch.valid){
        if(id_latch.num_stall == 0){
            // make id invalid only after num stalls = 0
            id_latch.valid = false;
        }
        return;
    }


    if(id_latch.branch_is_taken_resolved){
        // to kill the instruction in case of branch taken
        id_latch.valid = false;
        id_latch.branch_is_taken_resolved = false;
    }
    else{
        id_latch.valid = true; // after jump
    }


    if(if_latch.num_stall > 0){
        // push - in the instruction vector of strings
        if_latch.instruction->vec.back() = "-";
        if_latch.num_stall--;
        return;
    }
    else{
        if_latch.is_stall = false;
        if_latch.pc = old_pc;

        int n = instructionMemory.size();
        if(if_latch.pc >= (unsigned int)4*n){
            if_latch.valid = false;
            return;
        }
        //If not out of memory bound, push IF
        if_latch.instruction->vec.back() = "IF";

        
        Instruction* new_instr = getInstruction(if_latch.pc);
        if_latch.instruction = new_instr;
        
        //beq x12 x0 8
        //add x5 x4 x3
        //old_pc = 8 and pc = 8, but I want to go to next instruction
        if(old_pc == pc){
            old_pc += 4;
            pc += 8;
        }
        else{
            old_pc = pc;
            pc += 4;
        }
    }

    if(if_latch.is_first_stalled){
        if_latch.num_stall = id_latch.num_stall;
        if_latch.is_stall = true;
        if_latch.is_first_stalled = false;
    }
}

void Processor::decode() {
    if(!id_latch.valid){
        ex_latch.valid = false;
        //To terminate the Pipeline, propagate valid = false
        return;
    }

    if(id_latch.num_stall > 0){
        id_latch.instruction->vec.back() = "-";
        id_latch.num_stall--;

        if(id_latch.num_stall == 0){
            if(is_forwarded){
                // lw came in MEM stage now - read from MEM and resolve branch
                if(id_latch.instruction->rs1 == mem_latch.instruction->rd){
                    registers.writeRegister(mem_latch.instruction->rd, mem_latch.mem_read_data);
                }
                if(id_latch.instruction->rs2 == mem_latch.instruction->rd){
                    registers.writeRegister(mem_latch.instruction->rd, mem_latch.mem_read_data);
                }
            }
            // now resolve branch if applicable
            if(id_latch.instruction->controls.is_branch){
                if(resolveBranch(*(id_latch.instruction))){
                    pc = id_latch.pc + id_latch.instruction->imm;
                    old_pc = pc;
                    pc += 4;
                    // kill the next instruction where currently IF is being implemented
                    id_latch.branch_is_taken_resolved = true;
                }
            }
            // jump instruction
            if(id_latch.instruction->controls.is_jump){
                if(id_latch.instruction->opcode == 0x6F) { 
                    // jal
                    // update the ra value here
                    registers.writeRegister(id_latch.instruction->rd, id_latch.pc + 4);
    
                    pc = id_latch.pc + id_latch.instruction->imm;
                    id_latch.branch_is_taken_resolved = true;   
                    old_pc = pc;
                    pc += 4;
                } 
                else if(id_latch.instruction->opcode == 0x67) { 
                    // jalr
                    // to make sure the number is even
                    // update the ra value here
                    registers.writeRegister(id_latch.instruction->rd, id_latch.pc + 4);
                    int32_t op1 = registers.readRegister(id_latch.instruction->rs1);
                    old_pc = pc;
                    pc += 4;
                    pc = (op1 + id_latch.instruction->imm) & ~1;
                    id_latch.branch_is_taken_resolved = true;
                }
                ex_latch.is_jump = true; //debug Importnat
            }
        }
        return;
    }

    id_latch.is_stall = false;
    decodeInstruction(if_latch.instruction);

    //Data Hazards
    if(is_forwarded){
        forward(&if_latch, &id_latch, &ex_latch, &mem_latch, &wb_latch);
    }
    else{
        if(mem_latch.instruction != nullptr && !mem_latch.is_stall){
            if(checkDataHazardrs1(if_latch.instruction, mem_latch.instruction) || checkDataHazardrs2(if_latch.instruction, mem_latch.instruction)){
                // propogate the stalls
                if_latch.is_first_stalled = true;
                id_latch.is_stall = true;
                id_latch.num_stall = 1;
            }
        }
        //if both have hazard, then it will pick of ex
        if(ex_latch.instruction != nullptr && !ex_latch.is_stall){
            if(checkDataHazardrs1(if_latch.instruction, ex_latch.instruction) || checkDataHazardrs2(if_latch.instruction, ex_latch.instruction)){
                // propogate the stalls
                if_latch.is_first_stalled = true;
                id_latch.is_stall = true;
                id_latch.num_stall = 2;
            }
        }
        
    }
    
    //Control Hazards
    //Neither Forwarding, nor Stalling
    if(!id_latch.is_stall && !is_forwarded){ //debug
        //Conditional Branch
        if(if_latch.instruction->controls.is_branch){
            if(resolveBranch(*(if_latch.instruction))){
                pc = if_latch.pc + if_latch.instruction->imm;
                id_latch.branch_is_taken_resolved = true;
            }
        }
        //Unconditional Branch
        if(if_latch.instruction->controls.is_jump){
            if(if_latch.instruction->opcode == 0x6F) { 
                // jal
                // update the ra value here
                registers.writeRegister(if_latch.instruction->rd, if_latch.pc + 4); //pc+4 saved
                id_latch.branch_is_taken_resolved = true;
                pc = if_latch.pc + if_latch.instruction->imm;
            }
            else if(if_latch.instruction->opcode == 0x67) { 
                // jalr
                // to make sure the number is even
                // update the ra value here
                int32_t op1 = registers.readRegister(if_latch.instruction->rs1);
                registers.writeRegister(if_latch.instruction->rd, if_latch.pc + 4);
                id_latch.branch_is_taken_resolved = true;
                pc = (op1 + if_latch.instruction->imm) & ~1;
            }
            ex_latch.is_jump = true; //debug important
        }
    }

    //store data in id_latch even if any Data or Control hazard detected
    id_latch.instruction = if_latch.instruction;
    id_latch.instruction->vec.back() = "ID";
    id_latch.pc = if_latch.pc;
    ex_latch.valid = true;
}

void Processor::execute() {
    if(!ex_latch.valid){
        mem_latch.valid = false;
        return;
    }
    if(id_latch.is_stall){
        //propagating stalls to EX
        ex_latch.is_stall = true;
        ex_latch.num_stall = id_latch.num_stall;
    }

    if(ex_latch.num_stall > 0){
        ex_latch.num_stall--;
        return;
    }
    else{
        // alu output in the case of jal / jalr
        ex_latch.is_stall = false;
        int32_t op1 = registers.readRegister(id_latch.instruction->rs1);
        int32_t op2;

        if(id_latch.instruction->controls.AluSrc){
            op2 = id_latch.instruction->imm;
        }
        else{
            op2 = registers.readRegister(id_latch.instruction->rs2);
        }

        

        switch(id_latch.instruction->controls.AluOp){
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
    
        // branches resolved in ID stage
        // set alu_output to register read data in case of sw instruction
        ex_latch.pc=id_latch.pc;
        ex_latch.instruction=id_latch.instruction;
        ex_latch.instruction->vec.back() = "EX";
        ex_latch.memory_value = id_latch.rs2_readdata;
        mem_latch.valid = true;
    }
    //read data from id_latch
    //execute instruction
    //store data in ex_latch

    //update vector of strings with EX
}

void Processor::mem() { //debug
    if(!mem_latch.valid){
        wb_latch.valid = false;
        return;
    }
    if(ex_latch.is_stall){
        mem_latch.is_stall = true;
        mem_latch.num_stall = ex_latch.num_stall + 1;  // debug
    }
    if(mem_latch.num_stall > 0){
        mem_latch.num_stall--;
        return;
    }
    mem_latch.is_stall = false;
    //read data from ex_latch
    //read or write based on control signal from Memory
    // lw
    if(ex_latch.instruction->controls.MemRead){
        mem_latch.mem_read_data=memory.readMemory(ex_latch.alu_output);
       
    }
    // sw x3 0(x1)
    else if(ex_latch.instruction->controls.MemWrite){
        memory.writeMemory(ex_latch.alu_output, ex_latch.memory_value);
    }
    //store data in mem_latch
    mem_latch.instruction = ex_latch.instruction;
    mem_latch.instruction->vec.back() = "MEM";
    mem_latch.pc = ex_latch.pc;
    mem_latch.alu_output = ex_latch.alu_output;
    //update vector of strings with MEM
    wb_latch.valid = true;
}

void Processor::writeback() {
    if(!wb_latch.valid){
        return;
    }

    if(mem_latch.is_stall){
        wb_latch.is_stall = true;
        wb_latch.num_stall = mem_latch.num_stall + 1;
    }
    if(wb_latch.is_stall){
        if(wb_latch.num_stall > 0){
            wb_latch.num_stall--;
        }
        else if(wb_latch.num_stall == 0){
            wb_latch.is_stall = false;
            if(mem_latch.instruction->controls.RegWrite){
                // lw
                if(mem_latch.instruction->controls.MemtoReg){
                    registers.writeRegister(mem_latch.instruction->rd, mem_latch.mem_read_data);
                }
                // jalr
                else if(mem_latch.instruction->controls.is_jump){
                    registers.writeRegister(mem_latch.instruction->rd, mem_latch.pc+4); //jal jalr
                }
                else{
                    registers.writeRegister(mem_latch.instruction->rd, mem_latch.alu_output);
                }
            }

            wb_latch.pc=mem_latch.pc;
            mem_latch.instruction->vec.back() = "WB";
            wb_latch.write_data = mem_latch.alu_output; // debug
        }
        return;
    }
    // no stall
    wb_latch.is_stall = false;
    //read data from mem_latch
    //write data to register file
    if(mem_latch.instruction->controls.RegWrite){
        if(mem_latch.instruction->controls.MemtoReg){
            registers.writeRegister(mem_latch.instruction->rd, mem_latch.mem_read_data);
        }
        else if(mem_latch.instruction->controls.is_jump){
            registers.writeRegister(mem_latch.instruction->rd, mem_latch.pc+4); //jal jalr
        }
        else{
            registers.writeRegister(mem_latch.instruction->rd, mem_latch.alu_output);
        }
    }

    //store data in wb_latch
    wb_latch.pc=mem_latch.pc; //No need, but a standard practice
    mem_latch.instruction->vec.back() = "WB";
    wb_latch.write_data = mem_latch.alu_output;
}

Processor::Processor(bool is_forward) {
    cycles = 0;
    old_pc = 0;
    pc = 4;
    is_forwarded = is_forward;
    id_latch.branch_is_taken_resolved = false;
    if_latch.instruction = nullptr;
    id_latch.instruction = nullptr;
    ex_latch.instruction = nullptr;
    mem_latch.instruction = nullptr;
    wb_latch.instruction = nullptr;
    if_latch.is_first_stalled = false;
    if_latch.valid = true;
    id_latch.valid = ex_latch.valid = mem_latch.valid = wb_latch.valid = false;
    if_latch.is_stall = false;
    id_latch.is_stall = ex_latch.is_stall = mem_latch.is_stall = wb_latch.is_stall = false;
    if_latch.num_stall = id_latch.num_stall = ex_latch.num_stall = mem_latch.num_stall = wb_latch.num_stall = 0;
}