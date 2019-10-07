#ifndef opcodes_h

#define opcodes_h

#include "chip8.h"

#include <stdint.h>

static void OpCode0(Chip8State *state, uint16_t code);

static void OpCode1(Chip8State* state, uint16_t code);

static void OpCode2(Chip8State* state, uint16_t code);

static void OpCode3(Chip8State* state, uint16_t code);

static void OpCode4(Chip8State* state, uint16_t code);

static void OpCode5(Chip8State* state, uint16_t code);

static void OpCode6(Chip8State* state, uint16_t code);

static void OpCode7(Chip8State* state, uint16_t code);

static void OpCode8(Chip8State* state, uint16_t code);

static void OpCode9(Chip8State* state, uint16_t code);

static void OpCodeA(Chip8State* state, uint16_t code);

static void OpCodeB(Chip8State* state, uint16_t code);

static void OpCodeC(Chip8State* state, uint16_t code);

static void OpCodeD(Chip8State* state, uint16_t code);

static void OpCodeE(Chip8State* state, uint16_t code);

static void OpCodeF(Chip8State* state, uint16_t code);


#endif