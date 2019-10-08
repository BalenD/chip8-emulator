#include <stdint.h>
#include <stdlib.h>
#include "chip8.h"

const int BYTE = 64  * 32 / 8;

static void unimplementedInstrunction(Chip8State* state)
{
    printf("ERROR: unimplemented instruction");
    Disassembler(state->memory, state->programCounter);
    exit(1);
}

static void OpCode0(Chip8State *state, uint16_t code)
{
    switch (code & 0xF)
    {
        case 0xE0: 
            {
                // fill the  screen block of memory with 0
                // by a byte
                memset(state->screen, 0, BYTE);
                state->programCounter += 2;
            }
            break;
        case 0xEE:
            {
                // set program counter to address at top of stack
                state->programCounter = state->stack[state->stackPointer];

                state->stackPointer -= 1;
                state->programCounter += 2;
            }
            break;
        default: unimplementedInstrunction(state); break;
    }
}

static void OpCode1(Chip8State* state, uint16_t code)
{
    //  get last 12 bits (where the address to jump to is)
    uint16_t t = code & 0xFFF;
    if (t == state->programCounter)
    {
        printf("infinite loop, turning on halt flag");
        state->halt = 1;
    }
    state->programCounter = t;
}

static void OpCode2(Chip8State* state, uint16_t code)
{
    state->stackPointer += 1;
    //  puts the program counter on top fo the stack
    state->stack[state->stackPointer] = state->programCounter;
    //  set the program counter to the address (last 12 bit of the operation)
    state->programCounter = code & 0xFFF;
}

static void OpCode3(Chip8State* state, uint16_t code)
{
    uint8_t reg = (code & 0xF00) >> 8;
    uint8_t valTocompare = code & 0xFF;
    if (state->V[reg] ==  valTocompare)
    {
        state->programCounter += 2;
    }
    state->programCounter += 2;
}

static void OpCode4(Chip8State* state, uint16_t code)
{
    uint8_t req = (code & 0xF00) >> 8;
    uint8_t valTocompare = code & 0xFF;
    if (state->V[req] != valTocompare)
    {
        state->programCounter += 2;
    }
    state->programCounter += 2;
}

static void OpCode5(Chip8State* state, uint16_t code)
{
    uint8_t reg1 = (code & 0xF00) >> 8;
    uint8_t reg2 = (code & 0xF0) >> 4;
    if (state->V[reg1] == state->V[reg2])
    {
        state->programCounter += 2;
    }
    state->programCounter += 2;
}

static void OpCode6(Chip8State* state, uint16_t code)
{
    uint8_t reg = (code & 0xF00) >> 8;
    state->V[reg] = code & 0xFF;
    state->programCounter += 2;
}

static void OpCode7(Chip8State* state, uint16_t code)
{
    uint8_t reg = (code & 0xF00) >> 8;
    state->V[reg] += (code & 0xFF);
    state->programCounter += 2;
}

static void OpCode8(Chip8State* state, uint16_t code)
{
    int lastNib = code & 0xF;
    uint8_t vxReg = (code & 0xF00) >> 8;
    uint8_t vyReg = (code & 0xF0) >> 4;

    switch (lastNib)
    {
        case 0: state->V[vxReg] = state->V[vyReg]; break;
        case 1: state->V[vxReg] != state->V[vyReg]; break;
        case 2: state->V[vxReg] &= state->V[vyReg]; break;
        case 3: state->V[vxReg] ^= state->V[vyReg]; break;
        case 4:
            {
                uint16_t res = state->V[vxReg] + state->V[vyReg];
                //  if res is bigger than 255
                if (res & 0xFF)
                {
                    state->V[0xF] = 1;
                }
                else
                {
                    state->V[0xF] = 0;
                }
                state->V[vxReg] = res & 0xFF;
            }
            break;
        case 5:
            {
                //  subtract vy from vx
                state->V[vxReg] -= state->V[vyReg];
                // assign the borrow to 0xf
                // borrow will be 0 or 1 based on the conditional expression
                state->V[0xF] = (state->V[vxReg] > state->V[vyReg]);;
            }
            break;
        case 6:
            {
                // set the least significant bit of vx to 1 or 0  and assigg to vf
                state->V[0xF] = state->V[vxReg] & 0x1;
                // divide by 2 using bit shift
                state->V[vxReg] = state->V[vxReg] >> 1;
                
            }
            break;
        case 7:
            {
                state->V[0xF] = (state->V[vyReg] > state->V[vxReg]);
                state->V[vxReg] = state->V[vyReg] - state->V[vxReg];
            }
            break;
        case 0xE:
            {
                // if vx adnded by 1000 0000 is equal to 1000 0000
                //  set to vf
                state->V[0xF] = (0x80 == (state->V[vxReg] & 0x80));
                //  multiply by 2
                state->V[vxReg] = state->V[vxReg] << 1;
            }
            break;
        default: unimplementedInstrunction(state); break;
    }
    state->programCounter += 2;
}

static void OpCode9(Chip8State* state, uint16_t code)
{
    uint8_t vxReg = (code & 0xF00) >> 8;
    uint8_t vyReg = (code & 0xF0) >> 4;
    if (state->V[vxReg] != state->V[vyReg])
    {
        state->programCounter += 2;
    }
    state->programCounter += 2;
}

static void OpCodeA(Chip8State* state, int16_t code)
{
    state->I = code & 0xFFF;
    state->programCounter += 2;
}

static void OpCodeB(Chip8State* state, uint16_t code)
{
    state->programCounter = (code & 0xFFF) + state->V[0];
}

static void OpCodeC(Chip8State* state, uint16_t code)
{
    uint8_t xReg = (code & 0xF00) >> 8;
    uint8_t kk = code & 0xFF;
    // limit the random number to 8 bits by the AND operation, so it wont go abvoe 256
    state->V[xReg] = rand() & kk;
}

static void OpCodeD(Chip8State* state, uint16_t code)
{

}

static void OpCodeE(Chip8State* state, uint16_t code)
{
    switch (code & 0xF)
    {
        case 0x1:
            {

            }
            break;
        case 0xE:
            {
                
            }
            break;
    default: unimplementedInstrunction(state); break;
    }
} 