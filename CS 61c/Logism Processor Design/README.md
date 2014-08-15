Logism Processor Design (Logism)
=

The project was completed using Logism. It is similar to MIPS, but with 16-bit two-cycle processor with 4 registers. The processor also has a 2-stage pipeline:
1. Instruction Fetch: Fetch instruction from instruction memory.
2. Excecute: Instruction is decoded, executed, and written back to memory. 

The implemented instructions include: 

>add, sub, sllv, srlv, srav, slt, or, and, addp8, subp8, jal, j, jr, beq, addi, andi, ori, bne, sw, lui, lw, disp

This included the creation of data memory, an ALU (Arithmetic Logic Unit) that handles signed overflow, a cpu consisting of both the datapath and control of the processor, and output devices in the form of output displays. 

In case you do not have Logism, I've provided screenshots of my beautiful circuits in the image files. 