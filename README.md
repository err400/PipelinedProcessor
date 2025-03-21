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