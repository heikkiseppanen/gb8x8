#include "gb8x8.h"
#include "cpu.h"

#ifndef DEBUGGER
#define DEBUGGER
#endif

u8 debugger(u8 *buffer, registers *regs, operation *ops);

u8 buffer[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void *read_memory() {
    return buffer;
}

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

//op1 u8/u16
void dec(reg *af, void *op1, _Bool is_8bit) {
    --*(u16 *)op1;

    if (!is_8bit)
        return;

    if (((*(u8 *)op1 + 1) & 0b11110000) == (*(u8 *)op1 & 0b11110000))
        SET_H(af);
    if (*(u8 *)op1 == 0)
        SET_Z(af);
    SET_N(af);
}

//op1 u8/u16
void inc(reg *af, void *op1, _Bool is_8bit) {
    ++*(u16 *)op1;

    if (!is_8bit)
        return;

    if (((*(u8 *)op1 - 1) & 0b11110000) == (*(u8 *)op1 & 0b11110000))
        SET_H(af);
    if (*(u8 *)op1 == 0)
        SET_Z(af);
    SET_N(af);
}

//op1 u8/16, op2 u8/16
void ld(void *op1, void *op2) {
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

//op1 u8, op2 u8
static inline void res(void* op1, void *op2) {
    *(u8 *)op2 ^= (0x1 << *(u8 *)op1);
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

void sbc(reg *af, void *op1, void *op2) {
    u8 start = *(u8 *)op1;
    *(u8 *)op1 -= *(u8 *)op2 - (CHECK_C(af));

    SET_N(af);
    if (*(u8 *)op1 == 0)
        SET_Z(af);
    if ((start & 0b11110000) != (*(u8 *)op1 & 0b11110000))
        SET_H(af);
    if (*(u8 *)op2 + (CHECK_C(af)) > *(u8 *)op1)
        SET_C(af);
}

//op1 u8, op2 u8
static inline void set(void *op1, void *op2) {
    *(u8 *)op2 |= (1 << *(u8 *)op1);
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

void *get_operand(operand_name name, registers *regs) {
    if (name == NUL)
        return 0;
    if (name <= $HL)
        return get_register(name, regs);
    if (name <= $HLBP)
        return get_byte(name, regs);
    else
        return read_memory();
}

u8 execute_operation(registers *regs, operation op) {
    u8 cycles = op.cycles;
    void *op1 = get_operand(op.operand1, regs);
    void *op2 = get_operand(op.operand2, regs);

//A = Accumalator
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
        case CALL: /*TODO*/ break;
        case CCF: ccf(&regs->AF); break;
        case CP: cp(&regs->AF, op1, op2); break;
        case CPL: /*TODO*/ break;
        case DAA: /*TODO*/ break;
        case DEC: dec(&regs->AF, op1, op.operand1 <= $L); break;
        case DI: /*TODO*/ break;
        case EI: /*TODO*/ break;
        case HALT: /*TODO*/ break;
        case INC: inc(&regs->AF, op1, op.operand1 <= $L); break;
        case JP: /*TODO*/ break;
        case JR: /*TODO*/ break;
        case LD: 
            if (op.operand2 != $SP)
                ld(op1, op2);
            else if (op.operand1 == $HL)
                ld_signed(regs, 1/*TODO get_byte()*/);
            else
                ld_sp(op1, op2);
            hl_decrement_increment(op.operand1, op1);
            hl_decrement_increment(op.operand2, op2);
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
        case RES: res(op1, op2); break;
        case RET: /*TODO*/ break;
        case RETI: /*TODO*/ break;
        case RL: rl(&regs->AF, op1); break;
        case RLA: rla(&regs->AF); break;
        case RLC: rlc(&regs->AF, op1); break;
        case RLCA: rlca(&regs->AF); break;
        case RR: /*TODO*/ break;
        case RRA: rra(&regs->AF); break;
        case RRC: rrc(&regs->AF, op1); break;
        case RRCA: rrca(&regs->AF); break;
        case RST: /*TODO*/ break;
        case SBC: sbc(&regs->AF, op1, op2); break;
        case SCF: SET_C((&regs->AF)); RESET_N((&regs->AF)); RESET_H((&regs->AF)); break;
        case SET: set(op1, op2); break;
        // case example: /*TODO*/ break;
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
        operation op = operations[*(u8 *)read_memory()];
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