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

void adc(reg *af, u8 val) {
    RESET_N(af);
    u16 n = af->hl.hi + val + CHECK_C(af);

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
    return *(u16 *)r.r;
}

u8 execute_operation(operation *operations, registers *regs, operation op) {
    u8 first_byte;
    u8 second_byte;

    if (op.name == NOP)
        return 1;
    else if (op.name == ILL) {
        for(;;);
    }

    if (op.bytes > 1)
        first_byte = read_memory();
    if (op.bytes > 2)
        second_byte = read_memory();
    
    switch (op.name) {
        case ADC: 
            if (op.operand2 == $HL)
                adc(&regs->AF, get_byte(regs->HL));
            else if (op.operand2 > $L)
                adc(&regs->AF, first_byte);
            else
                adc(&regs->AF, ((reg *)regs + op.operand1)->r);
            break;
    }

    (void)first_byte;
    (void)second_byte;
    (void)operations;
    (void)regs;

    return 1 + (op.bytes - 1);
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