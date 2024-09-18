#include "gb8x8.h"
#include "cpu.h"

#ifndef DEBUGGER
#define DEBUGGER
#endif

u8 debugger(u8 *buffer, registers *regs, operation *ops);

u8 buffer[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

u8 read_memory() {
    return *buffer;
}

void adc(reg *af, u8 op1, u8 op2) {
    u16 n = op1 + op2 + (CHECK_C(af));

    RESET_N(af);
    if (n == 0)
        SET_Z(af);
    if (n > 0b1111)
        SET_H(af);
    if (n > 0b11111111)
        SET_C(af);
    af->hl.hi = n;
}

void add_8b(reg *af, u8 op1, u8 op2) {
    u16 n = op1 + op2;

    RESET_N(af);
    if (n == 0)
        SET_Z(af);
    if (n > 0b1111)
        SET_H(af);
    if (n > 0b11111111)
        SET_C(af);
    af->hl.hi = n;
}

void add_16b(reg *af, reg *hl, u16 op1, u16 op2) {
    u32 n = op1 + op2;

    RESET_N(af);
    if (n > 0b111111111111)
        SET_H(af);
    if (n > 0b1111111111111111)
        SET_C(af);
    hl->r = n;
}

void add_signed(reg *af, reg *sp, u16 op1, i8 op2) {
    u32 n = op1 + op2;

    RESET_Z(af);
    RESET_N(af);
    if (n > 0b1111)
        SET_H(af);
    if (n > 0b11111111)
        SET_C(af);

    sp->r = n;
}

//get the byte pointed to by the register
//TODO
u8 get_byte(reg r) {
    // return read_memory_from_address(r.r);
    return 0;
}

u16 get_8b_register(operand_name name, registers *regs) {
    switch (name) {
        case $A:
            return regs->AF.hl.hi;
        case $B:
            return regs->BC.hl.hi;
        case $C:
            return regs->BC.hl.lo;
        case $D:
            return regs->DE.hl.hi;
        case $E:
            return regs->DE.hl.lo;
        case $H:
            return regs->HL.hl.hi;
        case $L:
            return regs->HL.hl.lo;
        default:
            return 0;
    }
}

u16 get_16b_register(operand_name name, registers *regs) {
    switch (name) {
        case $BC:
            return regs->BC.r;
        case $DE:
            return regs->DE.r;
        case $H:
            return regs->HL.hl.lo;
        default:
            return 0;
    }
}

//TODO HL,[HL],[HLI],[HLD] ?_?
u16 get_operand(operand_name name, registers *regs) {
    if (name <= $L)
        return get_8b_register(name, regs);
    if (name <= $HL)
        return get_16b_register(name, regs);
    else if (name == $HL)
        return get_byte(regs->HL);
    else
        return read_memory();
}

u8 execute_operation(registers *regs, operation op) {
    u8 cycles = op.cycles;

    if (op.name == NOP)
        return 1;
    else if (op.name == ILL) {
        for(;;);
    }

    u16 op1 = get_operand(op.operand1, regs);
    u16 op2 = get_operand(op.operand2, regs);

    // ADD A,r8
    // ADD A,[HL]
    // ADD A,n8
    // ADD HL,r16
    // ADD HL,SP
    // ADD SP,e8

    // reg, u8
    // hl, u16

    switch (op.name) {
    case ADC: adc(&regs->AF, op1, op2); break;
    case ADD:
        if (op.operand1 == $A)
            add_8b(&regs->AF, op1, op2);
        else if (op.operand1 == $HL)
            add_16b(&regs->AF, &regs->HL, op1, op2);
        else
            add_signed(&regs->AF, &regs->SP, op1, op2);
        break;
        default: break;
    }

    (void)regs;

    return cycles;
}

void cpu(void) {
    registers regs = {0};
    u8 cycle_counter = 100;
    _Bool interrupt = 0;
    operation operations[512];

    create_op_table(operations);

    while (1) {
        operation op = operations[read_memory()];
        cycle_counter -= execute_operation(&regs, op);

        #ifdef DEBUGGER
        u8 instruction_cycles = 0;
        if (!instruction_cycles)
            instruction_cycles = debugger(buffer, &regs, operations);
        instruction_cycles--;
        #endif

        if (cycle_counter == 0) {
            cycle_counter += 100;
            //check interrupts
            if (interrupt)
                break;
        }
    }
}