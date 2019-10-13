#ifndef chip8_h
#define chip8_h

#include <stdint.h>

typedef struct Chip8State
{
    uint8_t V[16]; // 16 8-bit registers as written in wikipedia
    uint16_t I; // Memory address register
    uint16_t stackPointer; // points to topmost level of teh stack
    uint16_t programCounter; // used to store current executing address
    uint16_t stack [16]; // stack used to store addresses interpreter should return to
    uint8_t delay; // timer  for delay
    uint8_t sound; // timer  for sound
    uint8_t *memory;
    uint8_t *screen; // memory [0xF00]
    uint8_t key_state[16];
    uint8_t save_key_state[16];
    int waitingForKey;
    int drawing;
    uint8_t halt;
} Chip8State;

Chip8State* InitChip8(void);
void Emulate(Chip8State *state);
void loadGameToMemory(const char* filePath, Chip8State* state);
void Disassembler(uint8_t *codeBuffer, int pc);

#include "opcodes.h"

#endif