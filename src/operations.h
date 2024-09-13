#pragma once

#include "gb8x8.h"

typedef struct {
    char name[5];
    uint8_t bytes;
    uint8_t cycles;
    char operand1[5];
    char operand2[5];
} operation;

operation *create_op_table(void);