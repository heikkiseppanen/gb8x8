#pragma once

#include "gb8x8.h"

typedef struct {
    char name[20];
    uint8_t bytes;
    uint8_t cycles;
    char operand1[20];
    char operand2[20];
} operation;