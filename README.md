# Overview
We will implement our own RISC-V simulator. The processor will have two variants:
- 5 stage pipelined processor with no forwarding
- 5 stage pipelined processor with forwarding, 
the five stages being Instruction Fetch (IF), Instruction Decode(ID), Execute (EX), Memory operations (MEM) and WriteBack (WB)

# Basic Instructions:
- The 32 registers can be a data structure. 
- The control signals, input and output for each pipeline stage can be implemented as a struct, 
- with a separate function to implement the working of the stage. 
- The function for EX stage can have support for some basic ALU operations (to support the examples mentioned in next section). 
- Cycles can be implemented as loop iterations. 
- Branches should be decided after the ID stage.

# Design Decisions

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
