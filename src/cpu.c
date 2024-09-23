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

void and(reg *af, u8 op1, u8 op2) {
    af->hl.hi = op1 | op2;

    RESET_N(af);
    RESET_C(af);
    SET_H(af);
    if (af->hl.hi == 0)
        SET_Z(af);
}

void bit(reg *af, u8 op1, u8 op2) {
    RESET_N(af);
    SET_H(af);
    if (((op2 >> op1) & 0x01) == 0)
        SET_Z(af);
}

void ccf(reg *af) {
    RESET_N(af);
    RESET_H(af);
    af->hl.lo ^= 0b00010000;
}

void cp(reg *af, u8 op1, u8 op2) {
    u8 n = op1 - op2;

    SET_N(af);
    if (n == 0)
        SET_Z(af);
    if (op2 > op1 || (n & 0b11110000) != (op1 & 0b11110000))
        SET_H(af);
    if (op2 > op1)
        SET_C(af);
}

void dec(registers *regs, operand_name name) {
    u8 n = 0;
    switch (name) {
        case $A: n = --regs->AF.hl.hi; break;
        case $B: n = --regs->BC.hl.hi; break;
        case $C: n = --regs->BC.hl.lo; break;
        case $D: n = --regs->DE.hl.hi; break;
        case $E: n = --regs->DE.hl.lo; break;
        case $H: n = --regs->HL.hl.hi; break;
        case $L: n = --regs->HL.hl.lo; break;
        case $BC: --regs->BC.r; return;
        case $DE: --regs->DE.r; return;
        case $HL: --regs->HL.r; return;
        case $SP: --regs->HL.r; return;
        case $HLBP: /*TODO get byte and dec*/ break;
        default: break;
    }
    if (((n + 1) & 0b11110000) == (n & 0b11110000))
        SET_H((&regs->AF));
    if (n == 0)
        SET_Z((&regs->AF));
    SET_N((&regs->AF));
}

void inc(registers *regs, operand_name name) {
    u8 n = 0;
    switch (name) {
        case $A: n = ++regs->AF.hl.hi; break;
        case $B: n = ++regs->BC.hl.hi; break;
        case $C: n = ++regs->BC.hl.lo; break;
        case $D: n = ++regs->DE.hl.hi; break;
        case $E: n = ++regs->DE.hl.lo; break;
        case $H: n = ++regs->HL.hl.hi; break;
        case $L: n = ++regs->HL.hl.lo; break;
        case $BC: ++regs->BC.r; return;
        case $DE: ++regs->DE.r; return;
        case $HL: ++regs->HL.r; return;
        case $SP: ++regs->HL.r; return;
        case $HLBP: /*TODO get byte and inc*/ break;
        default: break;
    }
    if (((n - 1) & 0b11110000) == (n & 0b11110000))
        SET_H((&regs->AF));
    if (n == 0)
        SET_Z((&regs->AF));
    SET_N((&regs->AF));
}

void ld(registers *regs, operand_name name, u16 op1, u16 op2) {
    (void)op1;
    switch (name) {
        case $A: regs->AF.hl.hi = op2; break;
        case $B: regs->BC.hl.hi = op2; break;
        case $C: regs->BC.hl.lo = op2; break;
        case $D: regs->DE.hl.hi = op2; break;
        case $E: regs->DE.hl.lo = op2; break;
        case $H: regs->HL.hl.hi = op2; break;
        case $L: regs->HL.hl.lo = op2; break;
        case $BC: regs->BC.r = op2; break;
        case $DE: regs->DE.r = op2; break;
        case $HL: regs->HL.r = op2; break;
        case $SP: regs->HL.r = op2; break;
        case $CBP: /*TODO get byte set it*/ break;
        case $BCBP: /*TODO get byte set it*/ break;
        case $DEBP: /*TODO get byte set it*/ break;
        case $HLBP: /*TODO get byte set it*/ break;
        case $a8: /*TODO get byte set it*/ break;
        case $a16: /*TODO get byte set it*/ break;
        case $HLD: /*TODO get byte set it*/ break;
        case $HLI: /*TODO get byte set it*/ break;
        default: break;
    }
}

void ld_signed(registers *regs, i8 n) {
    RESET_Z((&regs->AF));
    RESET_N((&regs->AF));

    regs->HL.r = regs->SP.r + n;

    if ((regs->HL.r & 0b1111111111110000) != (regs->SP.r & 0b1111111111110000))
        SET_H((&regs->AF));
    if ((regs->HL.r & 0b1111111100000000) != (regs->SP.r & 0b1111111100000000))
        SET_C((&regs->AF));
}

void ld_sp(u16 op1, u16 op2) {
    (void)op1;
    (void)op2;
    //TODO
    //*op1 = op2 & 0xFF;
    //*(op1 + 1) = op2 >> 8;
}

void or(reg *af, u8 op2) {
    af->hl.hi |= op2;

    if (af->hl.hi == 0)
        RESET_Z(af);
    RESET_N(af);
    RESET_H(af);
    RESET_C(af);
}

void res(registers *regs, u8 bit, operand_name name) {
    switch (name) {
        case $A: regs->AF.hl.hi ^= (0x1 << bit); break;
        case $B: regs->BC.hl.hi ^= (0x1 << bit); break;
        case $C: regs->BC.hl.lo ^= (0x1 << bit); break;
        case $D: regs->DE.hl.hi ^= (0x1 << bit); break;
        case $E: regs->DE.hl.lo ^= (0x1 << bit); break;
        case $H: regs->HL.hl.hi ^= (0x1 << bit); break;
        case $L: regs->HL.hl.lo ^= (0x1 << bit); break;
        // case $HLBP: TODO
        default: break;
    }
}

void rl(registers *regs, operand_name name) {
    u8 *p;
    switch (name) {
        case $A: p = &regs->AF.hl.hi; break;
        case $B: p = &regs->BC.hl.hi; break;
        case $C: p = &regs->BC.hl.lo; break;
        case $D: p = &regs->DE.hl.hi; break;
        case $E: p = &regs->DE.hl.lo; break;
        case $H: p = &regs->HL.hl.hi; break;
        case $L: p = &regs->HL.hl.lo; break;
        // case $HLBP: TODO
        default: break;
    }

    _Bool carry_check = 0;
    if ((*p) & 0b10000000)
        carry_check = 1;
    (*p) <<= 1;
    (*p) |= CHECK_C((&regs->AF));

    RESET_C((&regs->AF));
    if (carry_check)
        SET_C((&regs->AF));
    if (*p == 0)
        SET_Z((&regs->AF));
    RESET_N((&regs->AF));
    RESET_H((&regs->AF));
}

void rla(reg *af) {
    _Bool carry_check = 0;
    if (af->hl.hi & 0b10000000)
        carry_check = 1;
    af->hl.hi <<= 1;
    af->hl.hi |= CHECK_C(af);

    RESET_C(af);
    RESET_N(af);
    RESET_H(af);
    RESET_Z(af);
    if (carry_check)
        SET_C(af);
    
}

u16 get_8b_register(operand_name name, registers *regs) {
    switch (name) {
        case $A: return regs->AF.hl.hi;
        case $B: return regs->BC.hl.hi;
        case $C: return regs->BC.hl.lo;
        case $D: return regs->DE.hl.hi;
        case $E: return regs->DE.hl.lo;
        case $H: return regs->HL.hl.hi;
        case $L: return regs->HL.hl.lo;
        default: return 0;
    }
}

u16 get_16b_register(operand_name name, registers *regs) {
    switch (name) {
        case $BC: return regs->BC.r;
        case $DE: return regs->DE.r;
        case $HL: return regs->HL.r;
        case $SP: return regs->SP.r;
        default: return 0;
    }
}

//get the byte pointed to by the register
//TODO
u8 get_byte(operand_name name, registers *regs) {
    (void)regs;
    switch (name) {
        // case $CBP:
        //     return read_memory_from_address(regs->BC.hl.lo);
        // case $BCBP:
        //     return read_memory_from_address(regs->BC.r);
        // case $DEBP:
        //     return read_memory_from_address(regs->DE.r);
        // case $HLBP:
        //     return read_memory_from_address(regs->HL.r);
        default:
            return 0;
    }
}

u16 get_operand(operand_name name, registers *regs) {
    if (name == NUL)
        return 0;
    if (name <= $L)
        return get_8b_register(name, regs);
    if (name <= $HL)
        return get_16b_register(name, regs);
    if (name <= $HLBP)
        return get_byte(name, regs);
    else
        return read_memory();
}

u8 execute_operation(registers *regs, operation op) {
    u8 cycles = op.cycles;
    u16 op1 = get_operand(op.operand1, regs);
    u16 op2 = get_operand(op.operand2, regs);

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
        case AND: and(&regs->AF, op1, op2); break;
        case BIT: bit(&regs->AF, op1, op2); break;
        case CALL: /*TODO*/ break;
        case CCF: ccf(&regs->AF); break;
        case CP: cp(&regs->AF, op1, op2); break;
        case CPL: /*TODO*/ break;
        case DAA: /*TODO*/ break;
        case DEC: dec(regs, op.operand1); break;
        case DI: /*TODO*/ break;
        case EI: /*TODO*/ break;
        case HALT: /*TODO*/ break;
        case INC: inc(regs, op.operand1); break;
        case JP: /*TODO*/ break;
        case JR: /*TODO*/ break;
        case LD: 
            if (op.operand2 != $SP)
                ld(regs, op.operand1, op1, op2);
            else if (op.operand1 == $HL)
                ld_signed(regs, 1/*get_byte()*/);
            else
                ld_sp(op1, op2);
            break;
        case LDH:
            // if (op.operand1 == $A)
            //     regs->AF.hl.hi = 0xFF00 + op2;
            // else
            //     memory[0xFF00 + op1] = regs->AF.hl.hi;
            break;
        case NOP: break;
        case ILL: for(;;); break;
        case OR: or(&regs->AF, op2); break;
        case POP: /*TODO*/ break;
        case PUSH: /*TODO*/ break;
        case RES: res(regs, op1, op.operand2); break;
        case RET: /*TODO*/ break;
        case RETI: /*TODO*/ break;
        case RL: rl(regs, op.operand1); break;
        case RLA: rla(&regs->AF); break;
        default: break;
    }
    return cycles;
}

void cpu(void) {
    registers regs = {0};
    u8 interrupt_counter = 100;
    operation operations[512];

    create_op_table(operations);

    while (1) {
        operation op = operations[read_memory()];
        interrupt_counter -= execute_operation(&regs, op);

        #ifdef DEBUGGER
        u8 instruction_cycles = 0;
        if (!instruction_cycles)
            instruction_cycles = debugger(buffer, &regs, operations);
        instruction_cycles--;
        #endif

        if (interrupt_counter == 0) {
            _Bool interrupt = 0;

            interrupt_counter += INTERRUPT_INTERVAL;
            //check interrupts
            if (interrupt)
                break;
        }
    }
}