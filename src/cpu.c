#include "gb8x8.h"

#ifndef DEBUGGER
#define DEBUGGER
#endif

void cpu() {
    // uint8_t buffer[10] = {''};
    uint8_t interrupt_counter = 100;
    _Bool interrupt = 0;

    while (1) {
        //do processor stuff
        #ifdef DEBUGGER
        debugger();
        #endif
        if (interrupt_counter == 0) {
            interrupt_counter = 100;
            //check interrupts
            if (interrupt)
                break;
        }
    }
}