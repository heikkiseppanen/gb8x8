#pragma once

#include "gb8x8.h"

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_OP(OP) \
    OP(NOP) \
    OP(LD) \
    OP(INC) \
    OP(DEC) \
    OP(RLCA) \
    OP(ADD) \
    OP(RRCA) \
    OP(STOP) \
    OP(RLA) \
    OP(JR) \
    OP(RRA) \
    OP(DAA) \
    OP(CPL) \
    OP(SCF) \
    OP(CCF) \
    OP(HALT) \
    OP(ADC) \
    OP(SUB) \
    OP(SBC) \
    OP(AND) \
    OP(XOR) \
    OP(OR) \
    OP(CP) \
    OP(RET) \
    OP(POP) \
    OP(JP) \
    OP(CALL) \
    OP(PUSH) \
    OP(RST) \
    OP(CB) \
    OP(ILL) \
    OP(RETI) \
    OP(LDH) \
    OP(DI) \
    OP(EI) \
    OP(RLC) \
    OP(RRC) \
    OP(RL) \
    OP(RR) \
    OP(SLA) \
    OP(SRA) \
    OP(SWAP) \
    OP(SRL) \
    OP(BIT) \
    OP(RES) \
    OP(SET)

typedef enum {
    FOREACH_OP(GENERATE_ENUM)
} operation_name;

#define FOREACH_OPERAND(OPERAND) \
    OPERAND(NUL) \
    OPERAND($BC) \
    OPERAND($n16) \
    OPERAND($A) \
    OPERAND($B) \
    OPERAND($n8) \
    OPERAND($a16) \
    OPERAND($SP) \
    OPERAND($HL) \
    OPERAND($C) \
    OPERAND($DE) \
    OPERAND($D) \
    OPERAND($e8) \
    OPERAND($E) \
    OPERAND($NZ) \
    OPERAND($H) \
    OPERAND($Z) \
    OPERAND($L) \
    OPERAND($NC) \
    OPERAND($$00) \
    OPERAND($$08) \
    OPERAND($$10) \
    OPERAND($$18) \
    OPERAND($$20) \
    OPERAND($$28) \
    OPERAND($$30) \
    OPERAND($$38) \
    OPERAND($a8) \
    OPERAND($AF) \
    OPERAND($0) \
    OPERAND($1) \
    OPERAND($2) \
    OPERAND($3) \
    OPERAND($4) \
    OPERAND($5) \
    OPERAND($6) \
    OPERAND($7)

typedef enum {
    FOREACH_OPERAND(GENERATE_ENUM)
} operand_name;

typedef struct {
    operation_name name;
    uint8_t bytes;
    uint8_t cycles;
    operand_name operand1;
    operand_name operand2;
} operation;

void create_op_table(operation *operations);