#pragma once

#include "gb8x8.h"

#include "operations.h"

typedef union {
    u16 r;
    struct {
        u8 hi;
        u8 lo;
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

#define SET_Z(AF) AF->hl.lo |= 0b10000000
#define SET_N(AF) AF->hl.lo |= 0b01000000
#define SET_H(AF) AF->hl.lo |= 0b00100000
#define SET_C(AF) AF->hl.lo |= 0b00010000
#define CHECK_Z(AF) AF->hl.lo & 0b10000000 != 0
#define CHECK_N(AF) AF->hl.lo & 0b01000000 != 0
#define CHECK_H(AF) AF->hl.lo & 0b00100000 != 0
#define CHECK_C(AF) AF->hl.lo & 0b00010000 != 0
#define RESET_Z(AF) AF->hl.lo &= 0b01111111
#define RESET_N(AF) AF->hl.lo &= 0b10111111
#define RESET_H(AF) AF->hl.lo &= 0b11011111
#define RESET_C(AF) AF->hl.lo &= 0b11101111

void cpu(void);