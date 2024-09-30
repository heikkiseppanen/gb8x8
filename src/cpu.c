#include "gb8x8.h"
#include "cpu.h"

//TODO remove later
#ifndef DEBUGGER
#define DEBUGGER
#endif

u8 debugger(u8 *buffer, registers *regs, operation *ops);
void push(reg *sp, void *op1);

//TODO -->
u8 code[200] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
u8 stack[200];

void *read_memory(u16 pc) {
    return &code[pc];
}

void *read_stack(u16 address) {
    return &stack[address];
}

void set_stack(u16 sp, u8 val) {
    stack[sp] = val;
}
// <-- TODO

static inline _Bool overflow_check_bit3(u8 start, u8 res) {
    return (start & 0b11110000) == (res & 0b11110000);
}

static inline _Bool overflow_check_bit7(u8 start, u8 res) {
    return res < start;
}

static inline _Bool overflow_check_bit11(u16 start, u16 res) {
    return (start & 0b1111000000000000) == (res & 0b1111000000000000);
}

static inline _Bool overflow_check_bit15(u16 start, u16 res) {
    return res < start;
}

static inline void hl_decrement_increment(operand_name name, void *op) {
    if (name == $HLD)
        --*(u16 *)op;
    else if (name == $HLI)
        ++*(u16 *)op;
}

//op1 u8, op2 u8
void adc(reg *af, void *op1, void *op2) {
    u8 start = *(u8 *)(op1);
    *(u8 *)(op1) += *(u8 *)(op2) + (CHECK_C(af));

    RESET_N(af);
    if (overflow_check_bit3(start, *(u8 *)(op1)))
        SET_H(af);
    if (overflow_check_bit7(start, *(u8 *)(op1)))
        SET_C(af);
    if (*(u8 *)(op1) == 0)
        SET_Z(af);
}

//op1 u8, op2 u8
void add_8b(reg *af, void *op1, void *op2) {
    u8 start = *(u8 *)(op1);
    *(u8 *)(op1) += *(u8 *)(op2);

    RESET_N(af);
    if (overflow_check_bit3(start, *(u8 *)(op1)))
        SET_H(af);
    if (overflow_check_bit7(start, *(u8 *)(op1)))
        SET_C(af);
    if (*(u8 *)(op1) == 0)
        SET_Z(af);
}

//op1 u16, op2 u16
void add_16b(reg *af, void *op1, void *op2) {
    u16 start = *(u16 *)(op1);
    *(u16 *)(op1) += *(u16 *)(op2);

    RESET_N(af);
    if (overflow_check_bit11(start, *(u16 *)(op1)))
        SET_H(af);
    if (overflow_check_bit15(start, *(u16 *)(op1)))
        SET_C(af);
}

//op1 u16, op2 i8
void add_signed(reg *af, void *op1, void *op2) {
    u16 start = *(u16 *)(op1);
    *(u16 *)(op1) += *(i8 *)(op2);

    if (overflow_check_bit3(start & 0xFF, *(u16 *)(op1) & 0xFF))
        SET_H(af);
    if (overflow_check_bit7(start & 0xFF, *(u16 *)(op1) & 0xFF))
        SET_C(af);
    RESET_Z(af);
    RESET_N(af);
}

//op1 u8, op2 u8
void and(reg *af, void *op1, void *op2) {
    //TODO figure out if AND is saved or not
    *(u8 *)op1 |= *(u8 *)op2;

    RESET_N(af);
    RESET_C(af);
    SET_H(af);
    if (*(u8 *)op1 == 0)
        SET_Z(af);
}

//op1 u3, op2 u8
void bit(reg *af, void *op1, void *op2) {
    RESET_N(af);
    SET_H(af);
    if (((*(u8 *)op2 >> *(u8 *)op1) & 0x01) == 0)
        SET_Z(af);
}

u8 call(reg *sp, reg *pc, void *op1, void *op2) {
    if (op2 == NULL) {
        ++pc->r;
        push(sp, pc);
        pc->r = *(u16 *)op1;
        --pc->r; // Incremented back later
    } else {
        if (op1 == (void *)1)
            return 0;
        ++pc->r;
        push(sp, pc);
        pc->r = *(u16 *)op2;
        --pc->r; // Incremented back later
        return 3;
    }
    return 0;
}

void ccf(reg *af) {
    RESET_N(af);
    RESET_H(af);
    af->hl.lo ^= 0b00010000;
}

//op1 u8, op2 u8
void cp(reg *af, void *op1, void *op2) {
    u8 n = *(u8 *)op1 - *(u8 *)op2;

    SET_N(af);
    if (n == 0)
        SET_Z(af);
    if (*(u8 *)op2 > *(u8 *)op1 || (n & 0b11110000) != (*(u8 *)op1 & 0b11110000))
        SET_H(af);
    if (*(u8 *)op2 > *(u8 *)op1)
        SET_C(af);
}

void cpl(reg *af) {
    af->hl.hi = ~af->hl.hi;
    SET_N(af);
    SET_H(af);
}

//op1 u8/u16
void dec(reg *af, void *op1, _Bool is_8bit) {
    if (!is_8bit) {
        --*(u16 *)op1;
        return;
    }
    --*(u8 *)op1;

    if (((*(u8 *)op1 + 1) & 0b11110000) == (*(u8 *)op1 & 0b11110000))
        SET_H(af);
    if (*(u8 *)op1 == 0)
        SET_Z(af);
    SET_N(af);
}

//op1 u8/u16
void inc(reg *af, void *op1, _Bool is_8bit) {
    if (!is_8bit) {
        ++*(u16 *)op1;
        return;
    }
    ++*(u8 *)op1;

    if (((*(u8 *)op1 - 1) & 0b11110000) == (*(u8 *)op1 & 0b11110000))
        SET_H(af);
    if (*(u8 *)op1 == 0)
        SET_Z(af);
    SET_N(af);
}

//op1 cc/u16, op2 nul/u16
u8 jp(reg *pc, void *op1, void *op2) {
    if (op2 == NULL) {
        pc->r = *(u16 *)op1;
        --pc->r; // Incremented back later
    }
    else {
        if (op1 == (void *)1)
            return 0;
        pc->r = *(u16 *)op2;
        --pc->r; // Incremented back later
        return 1;
    }
    return 0;
}

u8 jr(reg *pc, void *op1, void *op2) {
    if (op2 == NULL) {
        pc->r += *(i8 *)op1;
        --pc->r; // Incremented back later
    }
    else {
        if (op1 == (void *)1)
            return 0;
        pc->r += *(i8 *)op2;
        --pc->r; // Incremented back later
        return 1;
    }
    return 0;
}

//op1 u8, op2 u8
void ld_8bit(void *op1, void *op2) {
    *(u8 *)op1 = *(u8 *)op2;
}

//op1 u16, op2 u16
void ld_16bit(void *op1, void *op2) {
    *(u16 *)op1 = *(u16 *)op2;
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

//op1 u16, op2 u16
void ld_sp(void *op1, void *op2) {
    (void)op1;
    (void)op2;
    //TODO
    //*op1 = op2 & 0xFF;
    //*(op1 + 1) = op2 >> 8;
}

//op2 u8
void or(reg *af, void *op2) {
    af->hl.hi |= *(u8 *)op2;

    if (af->hl.hi == 0)
        RESET_Z(af);
    RESET_N(af);
    RESET_H(af);
    RESET_C(af);
}

//op1 u16
void pop(reg *sp, void *op1) {
    reg *r16 = op1;

    r16->hl.lo = *(u8 *)read_stack(sp->r);
    ++sp->r;
    r16->hl.hi = *(u8 *)read_stack(sp->r);
    ++sp->r;
}

//op1 u16
void push(reg *sp, void *op1) {
    reg *r16 = op1;

    --sp->r;
    set_stack(sp->r, r16->hl.hi);
    --sp->r;
    set_stack(sp->r, r16->hl.lo);
}

//op1 u8, op2 u8
static inline void res(void* op1, void *op2) {
    *(u8 *)op2 ^= (0x1 << *(u8 *)op1);
}

u8 ret(reg *sp, reg *pc, void *op1) {
    if (op1 == (void *)1)
        return 0;
    pop(sp, &pc->r);
    --pc->r; // Incremented back later
    if (op1 != NULL)
        return 3;
    return 0;
}

//op1 u8
void rl(reg *af, void *op1) {
    u8 *p = (u8 *)op1;

    _Bool carry_check = 0;
    if ((*p) & 0b10000000)
        carry_check = 1;
    (*p) <<= 1;
    (*p) |= (CHECK_C(af));

    RESET_C(af);
    if (carry_check)
        SET_C(af);
    if (*p == 0)
        SET_Z(af);
    RESET_N(af);
    RESET_H(af);
}

void rla(reg *af) {
    _Bool carry_check = 0;
    if (af->hl.hi & 0b10000000)
        carry_check = 1;
    af->hl.hi <<= 1;
    af->hl.hi |= (CHECK_C(af));

    RESET_C(af);
    RESET_N(af);
    RESET_H(af);
    RESET_Z(af);
    if (carry_check)
        SET_C(af);
}

void rlc(reg *af, void *op1) {
    u8 *p = (u8 *)op1;

    RESET_C(af);
    if ((*p) & 0b10000000)
        SET_C(af);
    (*p) <<= 1;
    if (CHECK_C(af))
        (*p) |= 1;
    if (*p == 0)
        SET_Z(af);
    RESET_N(af);
    RESET_H(af);
}

void rlca(reg *af) {
    RESET_C(af);
    if (af->hl.hi & 0b10000000)
        SET_C(af);
    af->hl.hi <<= 1;
    if (CHECK_C(af))
        af->hl.hi |= 1;
    RESET_Z(af);
    RESET_N(af);
    RESET_H(af);
}

void rr(reg *af, void *op1) {
    _Bool c_is_set = CHECK_C(af);
    RESET_C(af);
    if (*(u8 *)op1 & 0b1)
        SET_C(af);
    *(u8 *)op1 >>= 1;
    if (c_is_set)
        *(u8 *)op1 |= 0b10000000;
    if (*(u8 *)op1 == 0)
        RESET_Z(af);
    RESET_N(af);
    RESET_H(af);
}

void rra(reg *af) {
    _Bool c_is_set = CHECK_C(af);
    RESET_C(af);
    if (af->hl.hi & 0b1)
        SET_C(af);
    af->hl.hi >>= 1;
    if (c_is_set)
        af->hl.hi |= 0b10000000;
    RESET_Z(af);
    RESET_N(af);
    RESET_H(af);
}

void rrc(reg *af, void *op1) {
    u8 *p = (u8 *)op1;

    RESET_C(af);
    if ((*p) & 1)
        SET_C(af);
    (*p) >>= 1;
    if (CHECK_C(af))
        (*p) |= 0b10000000;
    if (*p == 0)
        SET_Z(af);
    RESET_N(af);
    RESET_H(af);
}

void rrca(reg *af) {
    RESET_C(af);
    if (af->hl.hi & 0b1)
        SET_C(af);
    af->hl.hi >>= 1;
    if (CHECK_C(af))
        af->hl.hi |= 0b10000000;
    RESET_Z(af);
    RESET_N(af);
    RESET_H(af);
}

//op1 u8, op2 u8
void sbc(reg *af, void *op1, void *op2) {
    u8 start = *(u8 *)op1;
    *(u8 *)op1 -= *(u8 *)op2 - (CHECK_C(af));

    SET_N(af);
    if (*(u8 *)op1 == 0)
        SET_Z(af);
    if ((start & 0b11110000) != (*(u8 *)op1 & 0b11110000))
        SET_H(af);
    if (*(u8 *)op2 + (CHECK_C(af)) > start)
        SET_C(af);
}

//op1 u8, op2 u8
static inline void set(void *op1, void *op2) {
    *(u8 *)op2 |= (1 << *(u8 *)op1);
}

//op1 u8
void sla(reg *af, void *op1) {
    RESET_C(af);
    if (*(u8 *)op1 & 0b10000000)
        SET_C(af);
    *(u8 *)op1 <<= 1;
    if (*(u8 *)op1 == 0)
        SET_Z(af);
    SET_N(af);
    SET_H(af);
}

//op1 u8
void sra(reg *af, void *op1) {
    _Bool is_bit7_set = *(u8 *)op1 & 0b10000000;

    RESET_C(af);
    if (*(u8 *)op1 & 1)
        SET_C(af);
    *(u8 *)op1 >>= 1;
    if (is_bit7_set)
        *(u8 *)op1 |= 0b10000000;
    if (*(u8 *)op1 == 0)
        SET_Z(af);
    RESET_N(af);
    RESET_H(af);
}

//op1 u8
void srl(reg *af, void *op1) {
    RESET_C(af);
    if (*(u8 *)op1 & 1)
        SET_C(af);
    *(u8 *)op1 >>= 1;
    if (*(u8 *)op1 == 0)
        SET_Z(af);
    RESET_N(af);
    RESET_H(af);
}

//op1 u8, op2 u8
void sub(reg *af, void *op1, void *op2) {
    u8 start = *(u8 *)op1;
    *(u8 *)op1 -= *(u8 *)op2;

    SET_N(af);
    if (*(u8 *)op1 == 0)
        SET_Z(af);
    if ((start & 0b11110000) != (*(u8 *)op1 & 0b11110000))
        SET_H(af);
    if (*(u8 *)op2 > start)
        SET_C(af);
}

void swap(reg *af, void *op1) {
    *(u8 *)op1 = (*(u8 *)op1 >> 4) | (*(u8 *)op1 << 4);
    if (*(u8 *)op1 == 0)
        SET_Z(af);
    RESET_N(af);
    RESET_H(af);
    RESET_C(af);
}

void xor(reg *af, void *op1, void *op2) {
    *(u8 *)op1 ^= *(u8 *)op2;
    if (*(u8 *)op1 == 0)
        SET_Z(af);
    RESET_N(af);
    RESET_H(af);
    RESET_C(af);
}

void *get_register(operand_name name, registers *regs) {
    switch (name) {
        case $A: return &regs->AF.hl.hi;
        case $B: return &regs->BC.hl.hi;
        case $C: return &regs->BC.hl.lo;
        case $D: return &regs->DE.hl.hi;
        case $E: return &regs->DE.hl.lo;
        case $H: return &regs->HL.hl.hi;
        case $L: return &regs->HL.hl.lo;
        case $BC: return &regs->BC.r;
        case $DE: return &regs->DE.r;
        case $HL: return &regs->HL.r;
        case $SP: return &regs->SP.r;
        default: return 0;
    }
}

//get the byte pointed to by the register
//TODO
void *get_byte(operand_name name, registers *regs) {
    switch (name) {
        case $CBP:
            return read_stack(regs->BC.hl.lo);
        case $BCBP:
            return read_stack(regs->BC.r);
        case $DEBP:
            return read_stack(regs->DE.r);
        case $HLBP:
            return read_stack(regs->HL.r);
        default:
            return 0;
    }
}

void *check_cond(operand_name name, registers *regs) {
    switch (name) {
        case $NZ: return !CHECK_Z((&regs->AF)) ? (void *)1 : (void *)2;
        case $Z: return CHECK_Z((&regs->AF)) ? (void *)1 : (void *)2;
        case $NC: return !CHECK_C((&regs->AF)) ? (void *)1 : (void *)2;
        case $SC: return CHECK_C((&regs->AF)) ? (void *)1 : (void *)2;
        default: return 0;
    }
}

void *get_operand(operand_name name, registers *regs) {
    if (name == NUL)
        return NULL;
    else if (name <= $HL)
        return get_register(name, regs);
    else if (name <= $HLBP)
        return get_byte(name, regs);
    else if (name <= $SC)
        return check_cond(name, regs);
    else
        return read_memory(++regs->PC.r);
}

u8 execute_operation(registers *regs, operation op) {
    u8 cycles = op.cycles;
    void *op1 = get_operand(op.operand1, regs);
    void *op2 = get_operand(op.operand2, regs);

    switch (op.name) {
        case ADC: adc(&regs->AF, op1, op2); break;
        case ADD:
            if (op.operand1 == $A)
                add_8b(&regs->AF, op1, op2);
            else if (op.operand1 == $HL)
                add_16b(&regs->AF, op1, op2);
            else
                add_signed(&regs->AF, op1, op2);
            break;
        case AND: and(&regs->AF, op1, op2); break;
        case BIT: bit(&regs->AF, op1, op2); break;
        case CALL: call(&regs->SP, &regs->PC, op1, op2); break;
        case CCF: ccf(&regs->AF); break;
        case CP: cp(&regs->AF, op1, op2); break;
        case CPL: cpl(&regs->AF); break;
        case DAA: /*TODO*/ break;
        case DEC: dec(&regs->AF, op1, op.operand1 <= $L); break;
        case DI: /*TODO*/ break;
        case EI: /*TODO*/ break;
        case HALT: /*TODO*/ break;
        case INC: inc(&regs->AF, op1, op.operand1 <= $L); break;
        case JP: cycles += jp(&regs->PC, op1, op2); break;
        case JR: cycles += jr(&regs->PC, op1, op2); break;
        case LD: 
            if (op.operand2 == $SP) {
                ld_sp(op1, op2);
            } else if (op.operand1 == $HL)
                ld_signed(regs, 1/*TODO get_byte()*/);
            else if (op.operand1 <= $L)
                ld_8bit(op1, op2);
            else
                ld_16bit(op1, op2);
            hl_decrement_increment(op.operand1, op1);
            hl_decrement_increment(op.operand2, op2);
            break;
        case LDH:
            if (op.operand1 == $A) {
                if (op.operand2 == $C)
                    regs->AF.hl.hi = *(u8 *)read_stack(0xff00 + *(u8 *)op2);
                else
                    regs->AF.hl.hi = *(u8 *)read_stack(*(u16 *)op2);
            } else {
                if (op.operand1 == $C)
                    *(u8 *)read_stack(0xff00 + *(u8 *)op2) = regs->AF.hl.hi;
                else
                    *(u8 *)read_stack(*(u16 *)op2) = regs->AF.hl.hi;
            }
            break;
        case NOP: break;
        case ILL: for(;;); break;
        case OR: or(&regs->AF, op2); break;
        case POP: pop(&regs->SP, op1); break;
        case PUSH: push(&regs->SP, op1); break;
        case RES: res(op1, op2); break;
        case RET: cycles += ret(&regs->SP, &regs->PC, op1); break;
        case RETI: /*TODO*/ break;
        case RL: rl(&regs->AF, op1); break;
        case RLA: rla(&regs->AF); break;
        case RLC: rlc(&regs->AF, op1); break;
        case RLCA: rlca(&regs->AF); break;
        case RR: rr(&regs->AF, op1); break;
        case RRA: rra(&regs->AF); break;
        case RRC: rrc(&regs->AF, op1); break;
        case RRCA: rrca(&regs->AF); break;
        case RST: /*TODO*/ break;
        case SBC: sbc(&regs->AF, op1, op2); break;
        case SCF: SET_C((&regs->AF)); RESET_N((&regs->AF)); RESET_H((&regs->AF)); break;
        case SET: set(op1, op2); break;
        case SLA: sla(&regs->AF, op1); break;
        case SRA: sra(&regs->AF, op1); break;
        case SRL: srl(&regs->AF, op1); break;
        case STOP: /*TODO*/ break;
        case SUB: sub(&regs->AF, op1, op2); break;
        case SWAP: swap(&regs->AF, op1); break;
        case XOR: xor(&regs->AF, op1, op2); break;
        default: break;
    }
    ++regs->PC.r;
    return cycles;
}

void cpu(void) {
    registers regs = {0};
    i8 interrupt_counter = 100;
    operation operations[512];

    regs.SP.r = 199;
    regs.PC.r = 0;

    create_op_table(operations);

    while (1) {
        operation op = operations[*(u8 *)read_memory(regs.PC.r)];
        if (op.name == CB)
            op = operations[*(u8 *)read_memory(regs.PC.r++)];
        interrupt_counter -= execute_operation(&regs, op);

        #ifdef DEBUGGER
        u8 instruction_cycles = 0;
        if (!instruction_cycles)
            instruction_cycles = debugger(code, &regs, operations);
        instruction_cycles--;
        #endif

        if (interrupt_counter <= 0) {
            interrupt_counter += INTERRUPT_INTERVAL;
            _Bool interrupt = 0;

            //check interrupts
            if (interrupt)
                break;
        }
    }
}