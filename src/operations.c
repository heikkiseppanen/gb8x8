#include "operations.h"

operation create_operation(operation_name name, uint8_t bytes, uint8_t cycles, operand_name operand1, operand_name operand2) {
    return (operation){.name=name, .bytes=bytes, .cycles=cycles, .operand1=operand1, .operand2=operand2};
}

void create_op_table(operation *operations) {
    operation ops[] = {
        create_operation(NOP, 1, 1, NUL, NUL),
        create_operation(LD, 3, 3, $BC, $n16),
        create_operation(LD, 1, 2, $BCBP, $A),
        create_operation(INC, 1, 2, $BC, NUL),
        create_operation(INC, 1, 1, $B, NUL),
        create_operation(DEC, 1, 1, $B, NUL),
        create_operation(LD, 2, 2, $B, $n8),
        create_operation(RLCA, 1, 1, NUL, NUL),
        create_operation(LD, 3, 5, $a16, $SP),
        create_operation(ADD, 1, 2, $HL, $BC),
        create_operation(LD, 1, 2, $A, $BCBP),
        create_operation(DEC, 1, 2, $BC, NUL),
        create_operation(INC, 1, 1, $C, NUL),
        create_operation(DEC, 1, 1, $C, NUL),
        create_operation(LD, 2, 2, $C, $n8),
        create_operation(RRCA, 1, 1, NUL, NUL),
        create_operation(STOP, 2, 1, $n8, NUL),
        create_operation(LD, 3, 3, $DE, $n16),
        create_operation(LD, 1, 2, $DEBP, $A),
        create_operation(INC, 1, 2, $DE, NUL),
        create_operation(INC, 1, 1, $D, NUL),
        create_operation(DEC, 1, 1, $D, NUL),
        create_operation(LD, 2, 2, $D, $n8),
        create_operation(RLA, 1, 1, NUL, NUL),
        create_operation(JR, 2, 3, $e8, NUL),
        create_operation(ADD, 1, 2, $HL, $DE),
        create_operation(LD, 1, 2, $A, $DEBP),
        create_operation(DEC, 1, 2, $DE, NUL),
        create_operation(INC, 1, 1, $E, NUL),
        create_operation(DEC, 1, 1, $E, NUL),
        create_operation(LD, 2, 2, $E, $n8),
        create_operation(RRA, 1, 1, NUL, NUL),
        create_operation(JR, 2, 2, $NZ, $e8),
        create_operation(LD, 3, 3, $HL, $n16),
        create_operation(LD, 1, 2, $HLI, $A),
        create_operation(INC, 1, 2, $HL, NUL),
        create_operation(INC, 1, 1, $H, NUL),
        create_operation(DEC, 1, 1, $H, NUL),
        create_operation(LD, 2, 2, $H, $n8),
        create_operation(DAA, 1, 1, NUL, NUL),
        create_operation(JR, 2, 2, $Z, $e8),
        create_operation(ADD, 1, 2, $HL, $HL),
        create_operation(LD, 1, 2, $A, $HLI),
        create_operation(DEC, 1, 2, $HL, NUL),
        create_operation(INC, 1, 1, $L, NUL),
        create_operation(DEC, 1, 1, $L, NUL),
        create_operation(LD, 2, 2, $L, $n8),
        create_operation(CPL, 1, 1, NUL, NUL),
        create_operation(JR, 2, 2, $NC, $e8),
        create_operation(LD, 3, 3, $SP, $n16),
        create_operation(LD, 1, 2, $HLD, $A),
        create_operation(INC, 1, 2, $SP, NUL),
        create_operation(INC, 1, 3, $HLBP, NUL),
        create_operation(DEC, 1, 3, $HLBP, NUL),
        create_operation(LD, 2, 3, $HLBP, $n8),
        create_operation(SCF, 1, 1, NUL, NUL),
        create_operation(JR, 2, 2, $C, $e8),
        create_operation(ADD, 1, 2, $HL, $SP),
        create_operation(LD, 1, 2, $A, $HLD),
        create_operation(DEC, 1, 2, $SP, NUL),
        create_operation(INC, 1, 1, $A, NUL),
        create_operation(DEC, 1, 1, $A, NUL),
        create_operation(LD, 2, 2, $A, $n8),
        create_operation(CCF, 1, 1, NUL, NUL),
        create_operation(LD, 1, 1, $B, $B),
        create_operation(LD, 1, 1, $B, $C),
        create_operation(LD, 1, 1, $B, $D),
        create_operation(LD, 1, 1, $B, $E),
        create_operation(LD, 1, 1, $B, $H),
        create_operation(LD, 1, 1, $B, $L),
        create_operation(LD, 1, 2, $B, $HLBP),
        create_operation(LD, 1, 1, $B, $A),
        create_operation(LD, 1, 1, $C, $B),
        create_operation(LD, 1, 1, $C, $C),
        create_operation(LD, 1, 1, $C, $D),
        create_operation(LD, 1, 1, $C, $E),
        create_operation(LD, 1, 1, $C, $H),
        create_operation(LD, 1, 1, $C, $L),
        create_operation(LD, 1, 2, $C, $HLBP),
        create_operation(LD, 1, 1, $C, $A),
        create_operation(LD, 1, 1, $D, $B),
        create_operation(LD, 1, 1, $D, $C),
        create_operation(LD, 1, 1, $D, $D),
        create_operation(LD, 1, 1, $D, $E),
        create_operation(LD, 1, 1, $D, $H),
        create_operation(LD, 1, 1, $D, $L),
        create_operation(LD, 1, 2, $D, $HLBP),
        create_operation(LD, 1, 1, $D, $A),
        create_operation(LD, 1, 1, $E, $B),
        create_operation(LD, 1, 1, $E, $C),
        create_operation(LD, 1, 1, $E, $D),
        create_operation(LD, 1, 1, $E, $E),
        create_operation(LD, 1, 1, $E, $H),
        create_operation(LD, 1, 1, $E, $L),
        create_operation(LD, 1, 2, $E, $HLBP),
        create_operation(LD, 1, 1, $E, $A),
        create_operation(LD, 1, 1, $H, $B),
        create_operation(LD, 1, 1, $H, $C),
        create_operation(LD, 1, 1, $H, $D),
        create_operation(LD, 1, 1, $H, $E),
        create_operation(LD, 1, 1, $H, $H),
        create_operation(LD, 1, 1, $H, $L),
        create_operation(LD, 1, 2, $H, $HLBP),
        create_operation(LD, 1, 1, $H, $A),
        create_operation(LD, 1, 1, $L, $B),
        create_operation(LD, 1, 1, $L, $C),
        create_operation(LD, 1, 1, $L, $D),
        create_operation(LD, 1, 1, $L, $E),
        create_operation(LD, 1, 1, $L, $H),
        create_operation(LD, 1, 1, $L, $L),
        create_operation(LD, 1, 2, $L, $HLBP),
        create_operation(LD, 1, 1, $L, $A),
        create_operation(LD, 1, 2, $HLBP, $B),
        create_operation(LD, 1, 2, $HLBP, $C),
        create_operation(LD, 1, 2, $HLBP, $D),
        create_operation(LD, 1, 2, $HLBP, $E),
        create_operation(LD, 1, 2, $HLBP, $H),
        create_operation(LD, 1, 2, $HLBP, $L),
        create_operation(HALT, 1, 1, NUL, NUL),
        create_operation(LD, 1, 2, $HLBP, $A),
        create_operation(LD, 1, 1, $A, $B),
        create_operation(LD, 1, 1, $A, $C),
        create_operation(LD, 1, 1, $A, $D),
        create_operation(LD, 1, 1, $A, $E),
        create_operation(LD, 1, 1, $A, $H),
        create_operation(LD, 1, 1, $A, $L),
        create_operation(LD, 1, 2, $A, $HLBP),
        create_operation(LD, 1, 1, $A, $A),
        create_operation(ADD, 1, 1, $A, $B),
        create_operation(ADD, 1, 1, $A, $C),
        create_operation(ADD, 1, 1, $A, $D),
        create_operation(ADD, 1, 1, $A, $E),
        create_operation(ADD, 1, 1, $A, $H),
        create_operation(ADD, 1, 1, $A, $L),
        create_operation(ADD, 1, 2, $A, $HLBP),
        create_operation(ADD, 1, 1, $A, $A),
        create_operation(ADC, 1, 1, $A, $B),
        create_operation(ADC, 1, 1, $A, $C),
        create_operation(ADC, 1, 1, $A, $D),
        create_operation(ADC, 1, 1, $A, $E),
        create_operation(ADC, 1, 1, $A, $H),
        create_operation(ADC, 1, 1, $A, $L),
        create_operation(ADC, 1, 2, $A, $HLBP),
        create_operation(ADC, 1, 1, $A, $A),
        create_operation(SUB, 1, 1, $A, $B),
        create_operation(SUB, 1, 1, $A, $C),
        create_operation(SUB, 1, 1, $A, $D),
        create_operation(SUB, 1, 1, $A, $E),
        create_operation(SUB, 1, 1, $A, $H),
        create_operation(SUB, 1, 1, $A, $L),
        create_operation(SUB, 1, 2, $A, $HLBP),
        create_operation(SUB, 1, 1, $A, $A),
        create_operation(SBC, 1, 1, $A, $B),
        create_operation(SBC, 1, 1, $A, $C),
        create_operation(SBC, 1, 1, $A, $D),
        create_operation(SBC, 1, 1, $A, $E),
        create_operation(SBC, 1, 1, $A, $H),
        create_operation(SBC, 1, 1, $A, $L),
        create_operation(SBC, 1, 2, $A, $HLBP),
        create_operation(SBC, 1, 1, $A, $A),
        create_operation(AND, 1, 1, $A, $B),
        create_operation(AND, 1, 1, $A, $C),
        create_operation(AND, 1, 1, $A, $D),
        create_operation(AND, 1, 1, $A, $E),
        create_operation(AND, 1, 1, $A, $H),
        create_operation(AND, 1, 1, $A, $L),
        create_operation(AND, 1, 2, $A, $HLBP),
        create_operation(AND, 1, 1, $A, $A),
        create_operation(XOR, 1, 1, $A, $B),
        create_operation(XOR, 1, 1, $A, $C),
        create_operation(XOR, 1, 1, $A, $D),
        create_operation(XOR, 1, 1, $A, $E),
        create_operation(XOR, 1, 1, $A, $H),
        create_operation(XOR, 1, 1, $A, $L),
        create_operation(XOR, 1, 2, $A, $HLBP),
        create_operation(XOR, 1, 1, $A, $A),
        create_operation(OR, 1, 1, $A, $B),
        create_operation(OR, 1, 1, $A, $C),
        create_operation(OR, 1, 1, $A, $D),
        create_operation(OR, 1, 1, $A, $E),
        create_operation(OR, 1, 1, $A, $H),
        create_operation(OR, 1, 1, $A, $L),
        create_operation(OR, 1, 2, $A, $HLBP),
        create_operation(OR, 1, 1, $A, $A),
        create_operation(CP, 1, 1, $A, $B),
        create_operation(CP, 1, 1, $A, $C),
        create_operation(CP, 1, 1, $A, $D),
        create_operation(CP, 1, 1, $A, $E),
        create_operation(CP, 1, 1, $A, $H),
        create_operation(CP, 1, 1, $A, $L),
        create_operation(CP, 1, 2, $A, $HLBP),
        create_operation(CP, 1, 1, $A, $A),
        create_operation(RET, 1, 2, $NZ, NUL),
        create_operation(POP, 1, 3, $BC, NUL),
        create_operation(JP, 3, 3, $NZ, $a16),
        create_operation(JP, 3, 4, $a16, NUL),
        create_operation(CALL, 3, 3, $NZ, $a16),
        create_operation(PUSH, 1, 4, $BC, NUL),
        create_operation(ADD, 2, 2, $A, $n8),
        create_operation(RST, 1, 4, $$00, NUL),
        create_operation(RET, 1, 2, $Z, NUL),
        create_operation(RET, 1, 4, NUL, NUL),
        create_operation(JP, 3, 3, $Z, $a16),
        create_operation(CB, 1, 1, NUL, NUL),
        create_operation(CALL, 3, 3, $Z, $a16),
        create_operation(CALL, 3, 3, $a16, NUL),
        create_operation(ADC, 2, 2, $A, $n8),
        create_operation(RST, 1, 4, $$08, NUL),
        create_operation(RET, 1, 2, $NC, NUL),
        create_operation(POP, 1, 3, $DE, NUL),
        create_operation(JP, 3, 3, $NC, $a16),
        create_operation(ILL, 1, 1, NUL, NUL),
        create_operation(CALL, 3, 3, $NC, $a16),
        create_operation(PUSH, 1, 4, $DE, NUL),
        create_operation(SUB, 2, 2, $A, $n8),
        create_operation(RST, 1, 4, $$10, NUL),
        create_operation(RET, 1, 2, $C, NUL),
        create_operation(RETI, 1, 4, NUL, NUL),
        create_operation(JP, 3, 3, $C, $a16),
        create_operation(ILL, 1, 1, NUL, NUL),
        create_operation(CALL, 3, 3, $C, $a16),
        create_operation(ILL, 1, 1, NUL, NUL),
        create_operation(SBC, 2, 2, $A, $n8),
        create_operation(RST, 1, 4, $$18, NUL),
        create_operation(LDH, 2, 3, $a8, $A),
        create_operation(POP, 1, 3, $HL, NUL),
        create_operation(LD, 1, 2, $CBP, $A),
        create_operation(ILL, 1, 1, NUL, NUL),
        create_operation(ILL, 1, 1, NUL, NUL),
        create_operation(PUSH, 1, 4, $HL, NUL),
        create_operation(AND, 2, 2, $A, $n8),
        create_operation(RST, 1, 4, $$20, NUL),
        create_operation(ADD, 2, 4, $SP, $e8),
        create_operation(JP, 1, 1, $HL, NUL),
        create_operation(LD, 3, 4, $a16, $A),
        create_operation(ILL, 1, 1, NUL, NUL),
        create_operation(ILL, 1, 1, NUL, NUL),
        create_operation(ILL, 1, 1, NUL, NUL),
        create_operation(XOR, 2, 2, $A, $n8),
        create_operation(RST, 1, 4, $$28, NUL),
        create_operation(LDH, 2, 3, $A, $a8),
        create_operation(POP, 1, 3, $AF, NUL),
        create_operation(LD, 1, 2, $A, $CBP),
        create_operation(DI, 1, 1, NUL, NUL),
        create_operation(ILL, 1, 1, NUL, NUL),
        create_operation(PUSH, 1, 4, $AF, NUL),
        create_operation(OR, 2, 2, $A, $n8),
        create_operation(RST, 1, 4, $$30, NUL),
        create_operation(LD, 2, 3, $HL, $SP),
        create_operation(LD, 1, 2, $SP, $HL),
        create_operation(LD, 3, 4, $A, $a16),
        create_operation(EI, 1, 1, NUL, NUL),
        create_operation(ILL, 1, 1, NUL, NUL),
        create_operation(ILL, 1, 1, NUL, NUL),
        create_operation(CP, 2, 2, $A, $n8),
        create_operation(RST, 1, 4, $$38, NUL),
        create_operation(RLC, 2, 2, $B, NUL),
        create_operation(RLC, 2, 2, $C, NUL),
        create_operation(RLC, 2, 2, $D, NUL),
        create_operation(RLC, 2, 2, $E, NUL),
        create_operation(RLC, 2, 2, $H, NUL),
        create_operation(RLC, 2, 2, $L, NUL),
        create_operation(RLC, 2, 4, $HLBP, NUL),
        create_operation(RLC, 2, 2, $A, NUL),
        create_operation(RRC, 2, 2, $B, NUL),
        create_operation(RRC, 2, 2, $C, NUL),
        create_operation(RRC, 2, 2, $D, NUL),
        create_operation(RRC, 2, 2, $E, NUL),
        create_operation(RRC, 2, 2, $H, NUL),
        create_operation(RRC, 2, 2, $L, NUL),
        create_operation(RRC, 2, 4, $HLBP, NUL),
        create_operation(RRC, 2, 2, $A, NUL),
        create_operation(RL, 2, 2, $B, NUL),
        create_operation(RL, 2, 2, $C, NUL),
        create_operation(RL, 2, 2, $D, NUL),
        create_operation(RL, 2, 2, $E, NUL),
        create_operation(RL, 2, 2, $H, NUL),
        create_operation(RL, 2, 2, $L, NUL),
        create_operation(RL, 2, 4, $HLBP, NUL),
        create_operation(RL, 2, 2, $A, NUL),
        create_operation(RR, 2, 2, $B, NUL),
        create_operation(RR, 2, 2, $C, NUL),
        create_operation(RR, 2, 2, $D, NUL),
        create_operation(RR, 2, 2, $E, NUL),
        create_operation(RR, 2, 2, $H, NUL),
        create_operation(RR, 2, 2, $L, NUL),
        create_operation(RR, 2, 4, $HLBP, NUL),
        create_operation(RR, 2, 2, $A, NUL),
        create_operation(SLA, 2, 2, $B, NUL),
        create_operation(SLA, 2, 2, $C, NUL),
        create_operation(SLA, 2, 2, $D, NUL),
        create_operation(SLA, 2, 2, $E, NUL),
        create_operation(SLA, 2, 2, $H, NUL),
        create_operation(SLA, 2, 2, $L, NUL),
        create_operation(SLA, 2, 4, $HLBP, NUL),
        create_operation(SLA, 2, 2, $A, NUL),
        create_operation(SRA, 2, 2, $B, NUL),
        create_operation(SRA, 2, 2, $C, NUL),
        create_operation(SRA, 2, 2, $D, NUL),
        create_operation(SRA, 2, 2, $E, NUL),
        create_operation(SRA, 2, 2, $H, NUL),
        create_operation(SRA, 2, 2, $L, NUL),
        create_operation(SRA, 2, 4, $HLBP, NUL),
        create_operation(SRA, 2, 2, $A, NUL),
        create_operation(SWAP, 2, 2, $B, NUL),
        create_operation(SWAP, 2, 2, $C, NUL),
        create_operation(SWAP, 2, 2, $D, NUL),
        create_operation(SWAP, 2, 2, $E, NUL),
        create_operation(SWAP, 2, 2, $H, NUL),
        create_operation(SWAP, 2, 2, $L, NUL),
        create_operation(SWAP, 2, 4, $HLBP, NUL),
        create_operation(SWAP, 2, 2, $A, NUL),
        create_operation(SRL, 2, 2, $B, NUL),
        create_operation(SRL, 2, 2, $C, NUL),
        create_operation(SRL, 2, 2, $D, NUL),
        create_operation(SRL, 2, 2, $E, NUL),
        create_operation(SRL, 2, 2, $H, NUL),
        create_operation(SRL, 2, 2, $L, NUL),
        create_operation(SRL, 2, 4, $HLBP, NUL),
        create_operation(SRL, 2, 2, $A, NUL),
        create_operation(BIT, 2, 2, $0, $B),
        create_operation(BIT, 2, 2, $0, $C),
        create_operation(BIT, 2, 2, $0, $D),
        create_operation(BIT, 2, 2, $0, $E),
        create_operation(BIT, 2, 2, $0, $H),
        create_operation(BIT, 2, 2, $0, $L),
        create_operation(BIT, 2, 3, $0, $HLBP),
        create_operation(BIT, 2, 2, $0, $A),
        create_operation(BIT, 2, 2, $1, $B),
        create_operation(BIT, 2, 2, $1, $C),
        create_operation(BIT, 2, 2, $1, $D),
        create_operation(BIT, 2, 2, $1, $E),
        create_operation(BIT, 2, 2, $1, $H),
        create_operation(BIT, 2, 2, $1, $L),
        create_operation(BIT, 2, 3, $1, $HLBP),
        create_operation(BIT, 2, 2, $1, $A),
        create_operation(BIT, 2, 2, $2, $B),
        create_operation(BIT, 2, 2, $2, $C),
        create_operation(BIT, 2, 2, $2, $D),
        create_operation(BIT, 2, 2, $2, $E),
        create_operation(BIT, 2, 2, $2, $H),
        create_operation(BIT, 2, 2, $2, $L),
        create_operation(BIT, 2, 3, $2, $HLBP),
        create_operation(BIT, 2, 2, $2, $A),
        create_operation(BIT, 2, 2, $3, $B),
        create_operation(BIT, 2, 2, $3, $C),
        create_operation(BIT, 2, 2, $3, $D),
        create_operation(BIT, 2, 2, $3, $E),
        create_operation(BIT, 2, 2, $3, $H),
        create_operation(BIT, 2, 2, $3, $L),
        create_operation(BIT, 2, 3, $3, $HLBP),
        create_operation(BIT, 2, 2, $3, $A),
        create_operation(BIT, 2, 2, $4, $B),
        create_operation(BIT, 2, 2, $4, $C),
        create_operation(BIT, 2, 2, $4, $D),
        create_operation(BIT, 2, 2, $4, $E),
        create_operation(BIT, 2, 2, $4, $H),
        create_operation(BIT, 2, 2, $4, $L),
        create_operation(BIT, 2, 3, $4, $HLBP),
        create_operation(BIT, 2, 2, $4, $A),
        create_operation(BIT, 2, 2, $5, $B),
        create_operation(BIT, 2, 2, $5, $C),
        create_operation(BIT, 2, 2, $5, $D),
        create_operation(BIT, 2, 2, $5, $E),
        create_operation(BIT, 2, 2, $5, $H),
        create_operation(BIT, 2, 2, $5, $L),
        create_operation(BIT, 2, 3, $5, $HLBP),
        create_operation(BIT, 2, 2, $5, $A),
        create_operation(BIT, 2, 2, $6, $B),
        create_operation(BIT, 2, 2, $6, $C),
        create_operation(BIT, 2, 2, $6, $D),
        create_operation(BIT, 2, 2, $6, $E),
        create_operation(BIT, 2, 2, $6, $H),
        create_operation(BIT, 2, 2, $6, $L),
        create_operation(BIT, 2, 3, $6, $HLBP),
        create_operation(BIT, 2, 2, $6, $A),
        create_operation(BIT, 2, 2, $7, $B),
        create_operation(BIT, 2, 2, $7, $C),
        create_operation(BIT, 2, 2, $7, $D),
        create_operation(BIT, 2, 2, $7, $E),
        create_operation(BIT, 2, 2, $7, $H),
        create_operation(BIT, 2, 2, $7, $L),
        create_operation(BIT, 2, 3, $7, $HLBP),
        create_operation(BIT, 2, 2, $7, $A),
        create_operation(RES, 2, 2, $0, $B),
        create_operation(RES, 2, 2, $0, $C),
        create_operation(RES, 2, 2, $0, $D),
        create_operation(RES, 2, 2, $0, $E),
        create_operation(RES, 2, 2, $0, $H),
        create_operation(RES, 2, 2, $0, $L),
        create_operation(RES, 2, 4, $0, $HLBP),
        create_operation(RES, 2, 2, $0, $A),
        create_operation(RES, 2, 2, $1, $B),
        create_operation(RES, 2, 2, $1, $C),
        create_operation(RES, 2, 2, $1, $D),
        create_operation(RES, 2, 2, $1, $E),
        create_operation(RES, 2, 2, $1, $H),
        create_operation(RES, 2, 2, $1, $L),
        create_operation(RES, 2, 4, $1, $HLBP),
        create_operation(RES, 2, 2, $1, $A),
        create_operation(RES, 2, 2, $2, $B),
        create_operation(RES, 2, 2, $2, $C),
        create_operation(RES, 2, 2, $2, $D),
        create_operation(RES, 2, 2, $2, $E),
        create_operation(RES, 2, 2, $2, $H),
        create_operation(RES, 2, 2, $2, $L),
        create_operation(RES, 2, 4, $2, $HLBP),
        create_operation(RES, 2, 2, $2, $A),
        create_operation(RES, 2, 2, $3, $B),
        create_operation(RES, 2, 2, $3, $C),
        create_operation(RES, 2, 2, $3, $D),
        create_operation(RES, 2, 2, $3, $E),
        create_operation(RES, 2, 2, $3, $H),
        create_operation(RES, 2, 2, $3, $L),
        create_operation(RES, 2, 4, $3, $HLBP),
        create_operation(RES, 2, 2, $3, $A),
        create_operation(RES, 2, 2, $4, $B),
        create_operation(RES, 2, 2, $4, $C),
        create_operation(RES, 2, 2, $4, $D),
        create_operation(RES, 2, 2, $4, $E),
        create_operation(RES, 2, 2, $4, $H),
        create_operation(RES, 2, 2, $4, $L),
        create_operation(RES, 2, 4, $4, $HLBP),
        create_operation(RES, 2, 2, $4, $A),
        create_operation(RES, 2, 2, $5, $B),
        create_operation(RES, 2, 2, $5, $C),
        create_operation(RES, 2, 2, $5, $D),
        create_operation(RES, 2, 2, $5, $E),
        create_operation(RES, 2, 2, $5, $H),
        create_operation(RES, 2, 2, $5, $L),
        create_operation(RES, 2, 4, $5, $HLBP),
        create_operation(RES, 2, 2, $5, $A),
        create_operation(RES, 2, 2, $6, $B),
        create_operation(RES, 2, 2, $6, $C),
        create_operation(RES, 2, 2, $6, $D),
        create_operation(RES, 2, 2, $6, $E),
        create_operation(RES, 2, 2, $6, $H),
        create_operation(RES, 2, 2, $6, $L),
        create_operation(RES, 2, 4, $6, $HLBP),
        create_operation(RES, 2, 2, $6, $A),
        create_operation(RES, 2, 2, $7, $B),
        create_operation(RES, 2, 2, $7, $C),
        create_operation(RES, 2, 2, $7, $D),
        create_operation(RES, 2, 2, $7, $E),
        create_operation(RES, 2, 2, $7, $H),
        create_operation(RES, 2, 2, $7, $L),
        create_operation(RES, 2, 4, $7, $HLBP),
        create_operation(RES, 2, 2, $7, $A),
        create_operation(SET, 2, 2, $0, $B),
        create_operation(SET, 2, 2, $0, $C),
        create_operation(SET, 2, 2, $0, $D),
        create_operation(SET, 2, 2, $0, $E),
        create_operation(SET, 2, 2, $0, $H),
        create_operation(SET, 2, 2, $0, $L),
        create_operation(SET, 2, 4, $0, $HLBP),
        create_operation(SET, 2, 2, $0, $A),
        create_operation(SET, 2, 2, $1, $B),
        create_operation(SET, 2, 2, $1, $C),
        create_operation(SET, 2, 2, $1, $D),
        create_operation(SET, 2, 2, $1, $E),
        create_operation(SET, 2, 2, $1, $H),
        create_operation(SET, 2, 2, $1, $L),
        create_operation(SET, 2, 4, $1, $HLBP),
        create_operation(SET, 2, 2, $1, $A),
        create_operation(SET, 2, 2, $2, $B),
        create_operation(SET, 2, 2, $2, $C),
        create_operation(SET, 2, 2, $2, $D),
        create_operation(SET, 2, 2, $2, $E),
        create_operation(SET, 2, 2, $2, $H),
        create_operation(SET, 2, 2, $2, $L),
        create_operation(SET, 2, 4, $2, $HLBP),
        create_operation(SET, 2, 2, $2, $A),
        create_operation(SET, 2, 2, $3, $B),
        create_operation(SET, 2, 2, $3, $C),
        create_operation(SET, 2, 2, $3, $D),
        create_operation(SET, 2, 2, $3, $E),
        create_operation(SET, 2, 2, $3, $H),
        create_operation(SET, 2, 2, $3, $L),
        create_operation(SET, 2, 4, $3, $HLBP),
        create_operation(SET, 2, 2, $3, $A),
        create_operation(SET, 2, 2, $4, $B),
        create_operation(SET, 2, 2, $4, $C),
        create_operation(SET, 2, 2, $4, $D),
        create_operation(SET, 2, 2, $4, $E),
        create_operation(SET, 2, 2, $4, $H),
        create_operation(SET, 2, 2, $4, $L),
        create_operation(SET, 2, 4, $4, $HLBP),
        create_operation(SET, 2, 2, $4, $A),
        create_operation(SET, 2, 2, $5, $B),
        create_operation(SET, 2, 2, $5, $C),
        create_operation(SET, 2, 2, $5, $D),
        create_operation(SET, 2, 2, $5, $E),
        create_operation(SET, 2, 2, $5, $H),
        create_operation(SET, 2, 2, $5, $L),
        create_operation(SET, 2, 4, $5, $HLBP),
        create_operation(SET, 2, 2, $5, $A),
        create_operation(SET, 2, 2, $6, $B),
        create_operation(SET, 2, 2, $6, $C),
        create_operation(SET, 2, 2, $6, $D),
        create_operation(SET, 2, 2, $6, $E),
        create_operation(SET, 2, 2, $6, $H),
        create_operation(SET, 2, 2, $6, $L),
        create_operation(SET, 2, 4, $6, $HLBP),
        create_operation(SET, 2, 2, $6, $A),
        create_operation(SET, 2, 2, $7, $B),
        create_operation(SET, 2, 2, $7, $C),
        create_operation(SET, 2, 2, $7, $D),
        create_operation(SET, 2, 2, $7, $E),
        create_operation(SET, 2, 2, $7, $H),
        create_operation(SET, 2, 2, $7, $L),
        create_operation(SET, 2, 4, $7, $HLBP),
        create_operation(SET, 2, 2, $7, $A)
        };

    for (int i = 0; i < 512; i++)
        operations[i] = ops[i];
}