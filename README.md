# Overview
We will implement our own RISC-V simulator. The processor will have two variants:
- 5 stage pipelined processor with no forwarding
- 5 stage pipelined processor with forwarding, 
the five stages being Instruction Fetch (IF), Instruction Decode(ID), Execute (EX), Memory operations (MEM) and WriteBack (WB)

# References
- [Berkeley Repo](https://github.com/ucb-bar/riscv-sodor/tree/master/src/main/scala/sodor/rv32_5stage)
