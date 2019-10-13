#ifndef opcodes_h

#define opcodes_h

#include "chip8.h"

#include <stdint.h>

void OpCode0(Chip8State *state, uint16_t code);

void OpCode1(Chip8State* state, uint16_t code);

void OpCode2(Chip8State* state, uint16_t code);

void OpCode3(Chip8State* state, uint16_t code);

void OpCode4(Chip8State* state, uint16_t code);

void OpCode5(Chip8State* state, uint16_t code);

void OpCode6(Chip8State* state, uint16_t code);

void OpCode7(Chip8State* state, uint16_t code);

void OpCode8(Chip8State* state, uint16_t code);

void OpCode9(Chip8State* state, uint16_t code);

void OpCodeA(Chip8State* state, uint16_t code);

void OpCodeB(Chip8State* state, uint16_t code);

void OpCodeC(Chip8State* state, uint16_t code);

void OpCodeD(Chip8State* state, uint16_t code);

void OpCodeE(Chip8State* state, uint16_t code);

void OpCodeF(Chip8State* state, uint16_t code);


#endif