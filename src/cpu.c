#include "gb8x8.h"
#include "cpu.h"

#ifndef DEBUGGER
#define DEBUGGER
#endif

uint8_t debugger(uint8_t *buffer, registers *regs, operation *ops);

uint8_t read_memory(uint8_t *buffer) {
    return *buffer;
}

void cpu(void) {
    uint8_t buffer[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    registers regs = {0};
    uint8_t cycle_counter = 100;
    _Bool interrupt = 0;
    uint8_t opcode = 0;
    operation operations[512];

    create_op_table(operations);

    while (1) {
        cycle_counter--;
        opcode = read_memory(buffer);
        operation op = operations[opcode];
        (void)op;

        #ifdef DEBUGGER
        uint8_t instruction_cycles = 0;
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