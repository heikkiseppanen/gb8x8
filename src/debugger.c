#include "gb8x8.h"
#include "cpu.h"
#include "operations.h"
#include "unistd.h"

static void display_instructions(uint8_t *buffer, operation *ops) {
    uint8_t i = 0;
    while(i < 16) {
        operation op = ops[*buffer];
        GB_FLOG(stdout, GB_TERM_BLUE, "0x%x\t%s", i, op.name);
        for (int byte_count = op.bytes; byte_count; byte_count--)
            buffer++;
        i++;
    }
}

static void display_registers(registers *regs) {
    (void)regs;
}

void debugger(uint8_t *buffer, registers *regs, operation *ops) {
    char input[1];
    read(0, input, 1);
    printf("input: %c\n", *input);
    switch (input[0]) {
        case 's':
            //next instruction
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