#pragma once

#include "gb8x8.h"

typedef union {
    uint16_t r;
    struct {
        uint8_t hi;
        uint8_t lo;
    } hl;
} Reg;

typedef struct {
    Reg AF;
    Reg BC;
    Reg DE;
    Reg HL;
    Reg SP;
    Reg PC;

} Registers;