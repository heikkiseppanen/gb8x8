#pragma once

#include "gb8x8.h"

typedef union {
    uint16_t r;
    struct {
        uint8_t hi;
        uint8_t lo;
    } hl;
} reg;

typedef struct {
    reg AF;
    reg BC;
    reg DE;
    reg HL;
    reg SP;
    reg PC;

} registers;