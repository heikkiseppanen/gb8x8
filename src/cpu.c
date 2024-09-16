#include "gb8x8.h"
#include "cpu.h"

#ifndef DEBUGGER
#define DEBUGGER
#endif

uint8_t debugger(uint8_t *buffer, registers *regs, operation *ops);

uint8_t buffer[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint8_t read_memory() {
    return *buffer;
}

uint8_t execute_operation(operation *operations, registers *regs, operation op) {
    int first_byte;
    int second_byte;

    if (op.bytes > 1)
        first_byte = read_memory();
    if (op.bytes > 2)
        second_byte = read_memory();

    (void)first_byte;
    (void)second_byte;
    (void)operations;
    (void)regs;
    
    return 1 + (op.bytes - 1);
}

void cpu(void) {
    registers regs = {0};
    uint8_t cycle_counter = 100;
    _Bool interrupt = 0;
    uint8_t opcode = 0;
    operation operations[512];

    create_op_table(operations);

    while (1) {
        opcode = read_memory();
        operation op = operations[opcode];
        cycle_counter -= execute_operation(operations, &regs, op);

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