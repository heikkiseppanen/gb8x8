#pragma once

#include "gb8x8.h"

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_OP(OP) \
    OP(NOP) \
    OP(ILL) \
    OP(ADC) \
    OP(ADD) \
    OP(LD) \
    OP(INC) \
    OP(DEC) \
    OP(RLCA) \
    OP(RRCA) \
    OP(STOP) \
    OP(RLA) \
    OP(RRA) \
    OP(DAA) \
    OP(CPL) \
    OP(SCF) \
    OP(CCF) \
    OP(HALT) \
    OP(SUB) \
    OP(SBC) \
    OP(AND) \
    OP(XOR) \
    OP(OR) \
    OP(CP) \
    OP(POP) \
    OP(PUSH) \
    OP(RST) \
    OP(CB) \
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
    OP(SET) \
    OP(CALL) \
    OP(JP) \
    OP(JR) \
    OP(RET) \

typedef enum {
    FOREACH_OP(GENERATE_ENUM)
} operation_name;

// operands $CBP, $BCBP, $DEBP, $HLBP
// BP ([r]) meaning byte pointed to by the register

#define FOREACH_OPERAND(OPERAND) \
    OPERAND($A) \
    OPERAND($B) \
    OPERAND($C) \
    OPERAND($D) \
    OPERAND($E) \
    OPERAND($H) \
    OPERAND($L) \
    OPERAND($BC) \
    OPERAND($DE) \
    OPERAND($HL) \
    OPERAND($AF) \
    OPERAND($SP) \
    OPERAND($HLI) \
    OPERAND($HLD) \
    OPERAND($CBP) \
    OPERAND($BCBP) \
    OPERAND($DEBP) \
    OPERAND($HLBP) \
    OPERAND($NZ) \
    OPERAND($Z) \
    OPERAND($NC) \
    OPERAND($a8) \
    OPERAND($e8) \
    OPERAND($n8) \
    OPERAND($a16) \
    OPERAND($n16) \
    OPERAND($$00) \
    OPERAND($$08) \
    OPERAND($$10) \
    OPERAND($$18) \
    OPERAND($$20) \
    OPERAND($$28) \
    OPERAND($$30) \
    OPERAND($$38) \
    OPERAND($0) \
    OPERAND($1) \
    OPERAND($2) \
    OPERAND($3) \
    OPERAND($4) \
    OPERAND($5) \
    OPERAND($6) \
    OPERAND($7) \
    OPERAND(NUL) \

typedef enum {
    FOREACH_OPERAND(GENERATE_ENUM)
} operand_name;

typedef struct {
    operation_name name;
    u8 bytes;
    u8 cycles;
    operand_name operand1;
    operand_name operand2;
} operation;

void create_op_table(operation *operations);