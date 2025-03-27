# RISC-V Simulator Project
### Authors
- Laksh Goel (2023CS10848)
- Yuvika Chaudhary (2023CS10353)
# Overview
We will implement our own RISC-V simulator. The processor will have two variants:
- 5 stage pipelined processor with no forwarding
- 5 stage pipelined processor with forwarding, 
the five stages being Instruction Fetch (IF), Instruction Decode(ID), Execute (EX), Memory operations (MEM) and WriteBack (WB)


# Design Decisions
### Architectural Choices
1. Pipeline Stages:
    - IF: Instruction Fetch
    - ID: Instruction Decode
    - EX: Execute
    - MEM: Memory Access
    - WB: Write Back
2. Implemented each pipeline stage as a separate struct to encapsulate stage-specific control signals, inputs, and outputs
3. Registers: Implemented a vector of 32 registers, each of 32 bit. All registers are initialized to 0.
4. Memory: Implemented a contiguous 4KB data memory, initialized with random values.
5. Provided read and write instruction for memory, keeping memory Private.
6. All the Instructions decoded using bitwise operations in InstrucyionFile.
### Pipeline Decisions
#### Data Hazards
1. Data Hazards are handled in ID stage by checking dependent registers in EX and MEM stages.
2. If a hazard is detected, the pipeline is stalled until the hazard is resolved.
#### Control Hazards
1. Branches resolved in the Instruction Decode (ID) stage using functions `resolveBranch` and `resolveUbranch`.
2. Default assumption: branches are not taken.
3. Instruction pipeline is killed and PC updated when branch is taken
4. Forwarding incorporated using functions `forward` and `forward_Dataflow` which look after possible stalling and forwarding of data.
#### Other Stage
1. Stalls are propogated in other stages in proper order using `is_stall` bool. Propagation starts from ID latch.
2. Memory can be read and written only in MEM stage.
3. Registers can be written in WB stage.

# Features

- 4KB data memory (initialized with random values)
- ALU operations supported and register values changed accordingly
- Branches resolved by reading registers in ID stage
- Appropriate stalls for forwarded and non-forwarded pipeline
- Instructions supported: 
    - R type instructions: add,sub,mul,div,and, or etc.   
    - I type instructions: lw/lb, sw/sb, addi etc
    - Jump instructions: jal and jalr
    - Branch instructions: beq, bne, blt, bge

# Assumptions
### Register Initialization:
- We have assumed that all the 32 registers (stored in a vector) are initialized with the value 0.
- For ALU Operations, we are performing ALU operations (as a real simulator would do) and storing it in appropriate registers.

### Branch and Jump statements
- Branch taken and jump statements are resolved in ID stage.
- For branching instructions (beq,bne,blt,bltu etc), we have assumed that branches will not be taken by default.
- If branch is taken, we kill the default instruction that was running and change the PC to the new instruction.
- Branches taken or not will be decided by the relevant register values.


### Load and Store Instructions
- We are currently supporting lw, sw instructions. 
- lw reads the data from the relevant address in Data memory whereas sw writes the data from address in 

### Data Memory Initialization
- Data Memory is of 4KB size.
- Values are assigned randomly in data memory.



# References and Sources
- [Berkeley Repo](https://github.com/ucb-bar/riscv-sodor/tree/master/src/main/scala/sodor/rv32_5stage)
- [C++ to RISC-V examples](https://marz.utk.edu/my-courses/cosc230/book/example-risc-v-assembly-programs/)
- Computer Organization and Design by David A. Patterson and John L. Hennessy
    - Chapter 2: Instructions: Language of the Computer
        - [RISC-V Instruction Format](assets/InstructionsFormat.png)

    - Chapter 4: The Processor
        - [Op-codes for R-type instructions (for ALU)](assets/operationsAndFields.png)
        - [Final DataPath and Control](assets/fina_datapath.png)
        - [Control Signals](assets/ControlSignals.png)
- Op-Code references for various Instructions taken from book and ChatGpt.
- We took help of Chatgpt to print data in stdout.