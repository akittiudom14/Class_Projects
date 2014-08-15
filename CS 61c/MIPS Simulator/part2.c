#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "mips.h" // for execute_syscall()
#include "types.h"

void execute_instruction(Instruction instruction,Processor* processor,Byte *memory) {
    
    switch(instruction.opcode) {
        case 0x0: // opcode == 0x0(SPECIAL)
            switch(instruction.rtype.funct) {
                case 0xc: // funct == 0xc (SYSCALL)
                    execute_syscall(processor);
                    processor->PC += 4;
                    break;
                case 0x24: // funct == 0x24 (AND)
                    processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs] & processor->R[instruction.rtype.rt];
                    processor->PC += 4;
                    break;
                case 0x0: // funct == 0x00 (sll)
                    processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rt] << instruction.rtype.shamt;
                    processor->PC += 4;
                    break;
                case 0x2: // funct == 0x02 (srl)
                    processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rt] >> instruction.rtype.shamt;
                    processor->PC += 4;
                    break;
                case 0x3: // funct == 0x03 (sra)
                    processor->R[instruction.rtype.rd] = (sWord) processor->R[instruction.rtype.rt] >> instruction.rtype.shamt;
                    processor->PC += 4;
                    break;
                case 0x8: // funct == 0x08 (jr)
                    processor->PC = processor->R[instruction.rtype.rs];
                    break;
                case 0x9: // funct == 0x09 (jalr)
                    {uint32_t tmp =  processor->PC + 4;
                    processor->PC = processor->R[instruction.rtype.rs];
                    processor->R[instruction.rtype.rd] = tmp;}
                    break;
                case 0x10: // funct == 0x10 (mfhi)
                    processor->R[instruction.rtype.rd] = processor->RHI; 
                    processor->PC += 4;
                    break;
                case 0x12: // funct == 0x10 (mflo)
                    processor->R[instruction.rtype.rd] = processor->RLO; 
                    processor->PC += 4;
                    break;
                case 0x18: // funct == 0x18 (mult)
                    {int64_t rs = (sWord) processor->R[instruction.rtype.rs];
                    int64_t rt = (sWord) processor->R[instruction.rtype.rt];
                    int64_t tmp = rs * rt;
                    processor->RHI = tmp >> 32;
                    processor->RLO = tmp & 0xffffffff;
                    processor->PC += 4;}
                    break;
                case 0x19: // funct == 0x19 (multu)
                    {uint64_t rs = processor->R[instruction.rtype.rs];
                    uint64_t rt = processor->R[instruction.rtype.rt];
                    uint64_t tmp = rs * rt;
                    processor->RHI = tmp >> 32;
                    processor->RLO = tmp & 0xffffffff;
                    processor->PC += 4;}
                    break;
                case 0x21: // funct == 0x21 (addu)
                    processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs] + processor->R[instruction.rtype.rt];
                    processor->PC += 4;
                    break;
                case 0x23: // funct == 0x23 (subu)
                    processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs] - processor->R[instruction.rtype.rt];
                    processor->PC += 4;
                    break;
                case 0x25: // funct == 0x25 (or)
                    processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs] | processor->R[instruction.rtype.rt];
                    processor->PC += 4;
                    break;
                case 0x26: // funct == 0x26 (xor)
                    processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs] ^ processor->R[instruction.rtype.rt];
                    processor->PC += 4;
                    break;
                case 0x27: // funct == 0x27 (nor)
                    processor->R[instruction.rtype.rd] = ~(processor->R[instruction.rtype.rs] | processor->R[instruction.rtype.rt]);
                    processor->PC += 4;
                    break;
                case 0x2a: // funct == 0x2a (slt)
                    processor->R[instruction.rtype.rd] = (sWord) processor->R[instruction.rtype.rs] < (sWord) processor->R[instruction.rtype.rt];
                    processor->PC += 4;
                    break;
                case 0x2b: // funct == 0x2b (sltu)
                    processor->R[instruction.rtype.rd] = processor->R[instruction.rtype.rs] < processor->R[instruction.rtype.rt];
                    processor->PC += 4;
                    break;
                default: // undefined funct
                    fprintf(stderr,"%s: pc=%08x,illegal function=%08x\n",__FUNCTION__,processor->PC,instruction.bits);
                    exit(-1);
                    break;
            }
            break;
        case 0x2: // opcode == 0x2 (J)
            processor->PC = ((processor->PC+4) & 0xf0000000) | (instruction.jtype.addr << 2);
            break;
        case 0xd: // opcode == 0xd (ORI)
            processor->R[instruction.itype.rt] = processor->R[instruction.itype.rs] | instruction.itype.imm;
            processor->PC += 4;
            break;
            case 0x3: // opcode == 0x03 (jal)
            processor->R[31] = processor->PC + 4;
            processor->PC = ((processor->PC+4) & 0xf0000000) | (instruction.jtype.addr << 2);
            break;
        case 0x4: // opcode == 0x04 (beq)
            if (processor->R[instruction.rtype.rs] == processor->R[instruction.rtype.rt]) {
                processor->PC = processor->PC + 4 + ( (int16_t) instruction.itype.imm) * 4;
            } else {
                processor->PC += 4;
            }
            break;
        case 0x5: // opcode == 0x05 (bne)
            if (processor->R[instruction.rtype.rs] != processor->R[instruction.rtype.rt]) {
                processor->PC = processor->PC + 4 + ( (int16_t) instruction.itype.imm) * 4;
            } else {
                processor->PC += 4;
            }
            break;
        case 0x9: // opcode == 0x09 (addiu)
            processor->R[instruction.itype.rt] = processor->R[instruction.itype.rs] + (sHalf) instruction.itype.imm;
            processor->PC += 4;
            break;
        case 0xa: // opcode == 0x0a (slti)
            processor->R[instruction.itype.rt] = (sWord) processor->R[instruction.itype.rs] < (sHalf) instruction.itype.imm;
            processor->PC += 4;
            break;
        case 0xb: // opcode == 0x0b (sltiu)
            processor->R[instruction.itype.rt] = processor->R[instruction.itype.rs] < (sHalf) instruction.itype.imm;
            processor->PC += 4;
            break;
        case 0xc: // opcode == 0x0c (andi)
            processor->R[instruction.itype.rt] = processor->R[instruction.itype.rs] & instruction.itype.imm;
            processor->PC += 4;
            break;
        case 0xe: // opcode == 0x0e (xori)
            processor->R[instruction.itype.rt] = processor->R[instruction.itype.rs] ^ instruction.itype.imm;
            processor->PC += 4;
            break;
        case 0xf: // opcode == 0x0f (lui)
            processor->R[instruction.itype.rt] = instruction.itype.imm << 16;
            processor->PC += 4;
            break;
        case 0x20: // opcode == 0x20 (lb)
            processor->R[instruction.itype.rt] = (int8_t) load(memory, (processor->R[instruction.itype.rs] + (int16_t)instruction.itype.imm), LENGTH_BYTE);
            processor->PC += 4;
            break;
        case 0x21: // opcode == 0x21 (lh)
            processor->R[instruction.itype.rt] = (int16_t) load(memory, processor->R[instruction.itype.rs] + (int16_t) instruction.itype.imm, LENGTH_HALF_WORD);
            processor->PC += 4;
            break;
        case 0x23: // opcode == 0x23 (lw)
            processor->R[instruction.itype.rt] = load(memory, processor->R[instruction.itype.rs] + (sHalf) instruction.itype.imm, LENGTH_WORD);
            processor->PC += 4;
            break;
        case 0x24: // opcode == 0x24 (lbu)
            processor->R[instruction.itype.rt] = (uint8_t) load(memory, processor->R[instruction.itype.rs] + (int16_t) instruction.itype.imm, LENGTH_BYTE);
            processor->PC += 4;
            break;
        case 0x25: // opcode == 0x25 (lhu)
            processor->R[instruction.itype.rt] = (uint16_t) load(memory, processor->R[instruction.itype.rs] + (sHalf) instruction.itype.imm, LENGTH_HALF_WORD);
            processor->PC += 4;
            break;
        case 0x28: // opcode == 0x28 (sb)
            store(memory, processor->R[instruction.itype.rs] + (sHalf) instruction.itype.imm, LENGTH_BYTE, (uint8_t) processor->R[instruction.itype.rt]);
            processor->PC += 4;
            break;
        case 0x29: // opcode == 0x29 (sh)
            store(memory, processor->R[instruction.itype.rs] + (sHalf) instruction.itype.imm, LENGTH_HALF_WORD, (uint16_t) processor->R[instruction.itype.rt]);
            processor->PC += 4;
            break;
        case 0x2b: // opcode == 0x2b (sw)
            store(memory, processor->R[instruction.itype.rs] + (sHalf) instruction.itype.imm, LENGTH_WORD, processor->R[instruction.itype.rt]);
            processor->PC += 4;
            break;

        default: // undefined opcode
            fprintf(stderr,"%s: pc=%08x,illegal instruction: %08x\n",__FUNCTION__,processor->PC,instruction.bits);
            exit(-1);
            break;
    }
}


int check(Address address,Alignment alignment) {
    // checks for valid addresses based on the alignment. Byte addresses dont have to be checked
    // out of range
    if (address < 1 || address > (MEMORY_SPACE - alignment + 1)) {
        return 0;
    }
    // check alignment of address
    if ((address % alignment) != 0) {
        return 0;
    }
    // if address is in bounds and aligned, return true
    return 1;
}

void store(Byte *memory,Address address,Alignment alignment,Word value) {
    if(!check(address,alignment)) {
        fprintf(stderr,"%s: bad write=%08x\n",__FUNCTION__,address);
        exit(-1);
    }
    if (alignment == LENGTH_BYTE) {
        *(Byte*)(memory+address) = value;
    } else if (alignment == LENGTH_HALF_WORD) {
        *(Half*)(memory+address) = value;
    } else {
        *(Word*)(memory+address) = value;
    }
}

Word load(Byte *memory,Address address,Alignment alignment) {
    if(!check(address,alignment)) {
        fprintf(stderr,"%s: bad read=%08x\n",__FUNCTION__,address);
        exit(-1);
    }
    if (alignment == LENGTH_BYTE) {
        return *((Word*)(memory+address)) & 0x000000ff;
    } else if (alignment == LENGTH_HALF_WORD) {
        return *((Word*)(memory+address)) & 0x0000ffff;
    } else {
        return *(Word*)(memory+address);
    }
}