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
    RESET_N(af);
    u16 n = op1 + op2 + (CHECK_C(af));

    if (n == 0)
        SET_Z(af);
    if (n > 0x07)
        SET_H(af);
    if (n > 0xFF)
        SET_C(af);
    af->hl.hi = n;
}

void add(reg *af, u8 val) {
    u16 n = af->hl.hi + val;

    if (n == 0)
        SET_Z(af);
    if (n > 0x07)
        SET_H(af);
    if (n > 0xFF)
        SET_C(af);
    af->hl.hi = n;
}

//get the byte pointed to by the register
u8 get_byte(reg r) {
    return *((u16 *)r.r);
}

u8 *get_r8_register(operand_name name, registers *regs) {
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

u8 get_operand(operand_name name, registers *regs) {
    if (name == $HL)
        return get_byte(regs->HL);
    else if (name > $L)
        return read_memory();
    else
        return get_r8_register(name, regs);
}

u8 execute_operation(operation *operations, registers *regs, operation op) {
    u8 cycles = op.cycles;

    if (op.name == NOP)
        return 1;
    else if (op.name == ILL) {
        for(;;);
    }

    u8 op1 = get_operand(op.operand1, regs);
    u8 op2 = get_operand(op.operand2, regs);

    switch (op.name) {
    case ADC:
        adc(&regs->AF, op1, op2);
        break;
        default:
        break;
    }

    (void)operations;
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
        cycle_counter -= execute_operation(operations, &regs, op);

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