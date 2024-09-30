#include "gb8x8.h"
#include "cpu.h"
#include "operations.h"
#include "unistd.h"

static void display_instructions(u8 *code, operation *ops) {
    u8 i = 0;
    static const char *OPERAND_NAMES[] = {
        FOREACH_OPERAND(GENERATE_STRING)
    };

    static const char *OP_NAMES[] = {
        FOREACH_OP(GENERATE_STRING)
    };

    while(i < 16) {
        operation op = ops[*code];

        if (op.operand2 != NUL)
            GB_FLOG(stdout, GB_TERM_BLUE, "0x%x\t%s%s\t%s, %s%s", i, GB_TERM_RED, OP_NAMES[op.name], OPERAND_NAMES[op.operand1], OPERAND_NAMES[op.operand2], GB_TERM_RESET);
        else if (op.operand1 != NUL)
            GB_FLOG(stdout, GB_TERM_BLUE, "0x%x\t%s%s\t%s%s", i, GB_TERM_RED, OP_NAMES[op.name], OPERAND_NAMES[op.operand1], GB_TERM_RESET);
        else
            GB_FLOG(stdout, GB_TERM_BLUE, "0x%x\t%s%s%s", i, GB_TERM_RED, OP_NAMES[op.name], GB_TERM_RESET);

        for (int byte_count = op.bytes; byte_count; byte_count--)
            code++;
        i++;
    }
}

static void display_registers(registers *regs) {
    GB_FLOG(stdout, GB_TERM_CYAN, "AF\t %d", regs->AF.r);
    GB_FLOG(stdout, GB_TERM_CYAN, "BC\t %d", regs->BC.r);
    GB_FLOG(stdout, GB_TERM_CYAN, "DE\t %d", regs->DE.r);
    GB_FLOG(stdout, GB_TERM_CYAN, "HL\t %d", regs->HL.r);
    GB_FLOG(stdout, GB_TERM_CYAN, "SP\t %d", regs->SP.r);
    GB_FLOG(stdout, GB_TERM_CYAN, "PC\t %d", regs->PC.r);
}

u8 debugger(u8 *code, registers *regs, operation *ops) {
    char input[1];

    while (1) {
        read(0, input, 1);
        printf("input: %c\n", *input);
        switch (input[0]) {
            case 's':
                return ops[*code].cycles;
                break;
            case 'r':
                display_registers(regs);
                //print registers
                break;
            case 'd':
                display_instructions(code, ops);
                //display instructions
                break;
        }
    }
    return 0;
}