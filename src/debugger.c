#include "gb8x8.h"
#include "cpu.h"
#include "operations.h"
#include "unistd.h"

static void display_instructions(uint8_t *buffer, operation *ops) {
    uint8_t i = 0;
    while(i < 16) {
        operation op = ops[*buffer];
        GB_FLOG(stdout, GB_TERM_BLUE, "0x%x\t%s%s", i, GB_TERM_RED, op.name);
        for (int byte_count = op.bytes; byte_count; byte_count--)
            buffer++;
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

uint8_t debugger(uint8_t *buffer, registers *regs, operation *ops) {
    char input[1];

    while (1) {
        read(0, input, 1);
        printf("input: %c\n", *input);
        switch (input[0]) {
            case 's':
                return ops[*buffer].cycles;
                break;
            case 'r':
                display_registers(regs);
                //print registers
                break;
            case 'd':
                display_instructions(buffer, ops);
                //display instructions
                break;
        }
    }
    return 0;
}