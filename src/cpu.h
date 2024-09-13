#pragma once

#include "gb8x8.h"

#include "operations.h"

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

#define SET_Z(AF) AF.hl.lo | 0b10000000
#define SET_N(AF) AF.hl.lo | 0b01000000
#define SET_H(AF) AF.hl.lo | 0b00100000
#define SET_C(AF) AF.hl.lo | 0b00010000
#define CHECK_Z(AF) AF.hl.lo & 0b10000000 != 0
#define CHECK_N(AF) AF.hl.lo & 0b01000000 != 0
#define CHECK_H(AF) AF.hl.lo & 0b00100000 != 0
#define CHECK_C(AF) AF.hl.lo & 0b00010000 != 0

void cpu(void);