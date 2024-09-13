#include "gb8x8.h"
#include "cpu.h"
#include "unistd.h"

static void display_instructions(uint8_t *buffer) {
    uint8_t i = 0;
    while(i < 16) {
        GB_FLOG(stdout, GB_TERM_BLUE, "0x%x\t", i);
        switch (*buffer) {
            case 0x00:
                GB_FLOG(stdout, GB_TERM_RED, "NOP");
                break;
        }
        buffer++;
        i++;
    }
}

static void display_registers(registers *regs) {
    (void)regs;
}

void debugger(uint8_t *buffer, registers *regs) {
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
            display_instructions(buffer);
            //display instructions
            break;
    }
}