#include "gb8x8.h"
#include "cpu.h"

#ifndef DEBUGGER
#define DEBUGGER
#endif

void debugger(uint8_t *buffer, registers *regs);

uint8_t read_memory(uint8_t *buffer) {
    return *buffer;
}

void cpu(void) {
    uint8_t buffer[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    registers regs = {0};
    uint8_t cycle_counter = 100;
    _Bool interrupt = 0;
    uint8_t opcode = 0;

    while (1) {
        cycle_counter--;
        opcode = read_memory(buffer);
        switch (opcode)
            case 0x00:
                break;

        #ifdef DEBUGGER
        debugger(buffer, &regs);
        #endif

        if (cycle_counter == 0) {
            cycle_counter += 100;
            //check interrupts
            if (interrupt)
                break;
        }
    }
}