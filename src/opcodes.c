#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "chip8.h"
#include "font.h"

const int BYTE = 64  * 32 / 8;

void unimplementedInstrunction(Chip8State* state)
{
    printf("ERROR: unimplemented instruction");
    Disassembler(state->memory, state->programCounter);
    exit(1);
}

void OpCode0(Chip8State *state, uint16_t code)
{
    switch (code & 0xF)
    {
        case 0xE0: 
            {
                // fill the  screen block of memory with 0
                // by a byte
                memset(state->screen, 0, BYTE);
                state->drawing = 1;
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

void OpCode1(Chip8State* state, uint16_t code)
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

void OpCode2(Chip8State* state, uint16_t code)
{
    state->stackPointer += 1;
    //  puts the program counter on top fo the stack
    state->stack[state->stackPointer] = state->programCounter;
    //  set the program counter to the address (last 12 bit of the operation)
    state->programCounter = code & 0xFFF;
}

void OpCode3(Chip8State* state, uint16_t code)
{
    uint8_t reg = (code & 0xF00) >> 8;
    uint8_t valTocompare = code & 0xFF;
    if (state->V[reg] ==  valTocompare)
    {
        state->programCounter += 2;
    }
    state->programCounter += 2;
}

void OpCode4(Chip8State* state, uint16_t code)
{
    uint8_t req = (code & 0xF00) >> 8;
    uint8_t valTocompare = code & 0xFF;
    if (state->V[req] != valTocompare)
    {
        state->programCounter += 2;
    }
    state->programCounter += 2;
}

void OpCode5(Chip8State* state, uint16_t code)
{
    uint8_t reg1 = (code & 0xF00) >> 8;
    uint8_t reg2 = (code & 0xF0) >> 4;
    if (state->V[reg1] == state->V[reg2])
    {
        state->programCounter += 2;
    }
    state->programCounter += 2;
}

void OpCode6(Chip8State* state, uint16_t code)
{
    uint8_t reg = (code & 0xF00) >> 8;
    state->V[reg] = code & 0xFF;
    state->programCounter += 2;
}

void OpCode7(Chip8State* state, uint16_t code)
{
    uint8_t reg = (code & 0xF00) >> 8;
    state->V[reg] += (code & 0xFF);
    state->programCounter += 2;
}

void OpCode8(Chip8State* state, uint16_t code)
{
    int lastNib = code & 0xF;
    uint8_t vxReg = (code & 0xF00) >> 8;
    uint8_t vyReg = (code & 0xF0) >> 4;

    switch (lastNib)
    {
        case 0: state->V[vxReg] = state->V[vyReg]; break;
        case 1: state->V[vxReg] |= state->V[vyReg]; break;
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

void OpCode9(Chip8State* state, uint16_t code)
{
    uint8_t vxReg = (code & 0xF00) >> 8;
    uint8_t vyReg = (code & 0xF0) >> 4;
    if (state->V[vxReg] != state->V[vyReg])
    {
        state->programCounter += 2;
    }
    state->programCounter += 2;
}

void OpCodeA(Chip8State* state, uint16_t code)
{
    state->I = code & 0xFFF;
    state->programCounter += 2;
}

void OpCodeB(Chip8State* state, uint16_t code)
{
    state->programCounter = (code & 0xFFF) + state->V[0];
    
}

void OpCodeC(Chip8State* state, uint16_t code)
{
    uint8_t xReg = (code & 0xF00) >> 8;
    uint8_t kk = code & 0xFF;
    // limit the random number to 8 bits by the AND operation, so it wont go abvoe 256
    state->V[xReg] = rand() & kk;
    state->programCounter += 2;
}

void OpCodeD(Chip8State* state, uint16_t code)
{
    // sprite to display
    int height = code & 0xf;
    int x = state->V[(code & 0xf00) >> 8];
    int y = state->V[(code & 0xf0) >> 4];
    unsigned short pixel;
    state->V[0xf] = 0;

    for (int yline = 0; yline < height; yline++)
    {
        pixel = state->memory[state->I + yline];
        
        for (int xline = 0; xline < 8; xline++)
        {
            if ((pixel & (0x80 >> xline)) != 0)
            {
                if (state->screen[(x + xline + ((y + yline) * 64))] == 1)
                {
                    state->V[0xF] = 1;
                }
                state->screen[x + xline + ((y + yline) * 64)] ^= 1;
            }
        }
        
    }
    

    // for (int i = 0; i < lines; i++)
    // {
    //     //  read n bytes from memory starting at i
    //     uint8_t *sprite = &state->memory[state->I + i];
    //     int spriteBit = 2;
    //     // display the n bytes at cordinats x and y
    //     for (int j = x; j < (x + 8) && j<64; j++)
    //     {
            
    //         int jover8 = j / 8;
    //         int jmod8 = j % 8;
    //         uint8_t srcBit = (*sprite >> spriteBit) & 0x1;

    //         if (srcBit)
    //         {
    //             uint8_t *desByte_p = &state->screen[(i + y) * (64/8) + jover8];
    //             uint8_t desByte = *desByte_p;
    //             uint8_t destMask = (0x80 >> jover8);
    //             uint8_t destBit = desByte & destMask;

    //             srcBit = srcBit << (7-jmod8);

    //             if (srcBit & destBit)
    //             {
    //                 state->V[0xF] = 1;
    //             }

    //             destBit ^= srcBit;
    //             desByte = (desByte & ~destMask) | destBit;

    //             *desByte_p = desByte;
    //         }
    //         spriteBit -= 1;
    //     }
        
    // }
    state->drawing = 1;
    state->programCounter += 2;

}

void OpCodeE(Chip8State* state, uint16_t code)
{
    uint8_t reg = (code & 0xF00) >> 8;
    switch (code & 0xF0)
    {
        case 0x9E:
            {
                //  skip next instruction if key vx is pressed
                if (state->key_state[state->V[reg]] != 0)
                {
                    state->programCounter += 2;
                }
            }
            break;
        case 0xA1:
            {
                //  skip next instruction if key vx is not pressed
                if (state->key_state[state->V[reg]] == 0)
                {
                    state->programCounter += 2;
                }
            }
            break;
        default: unimplementedInstrunction(state); break;
    }
}

void OpCodeF(Chip8State* state, uint16_t code)
{
    uint8_t reg = (code & 0xF00) >> 8;
    switch (code & 0xF)
    {
        case 0x07: state->V[reg] = state->delay; break;
        case 0x0A:
            {

                if (state->waitingForKey == 0)
                {
                    memcpy(&state->save_key_state, &state->key_state, 16);
                    state->waitingForKey = 1;
                    return;
                }
                else
                {
                    for (int i = 0; i < 16; i++)
                    {
                        if ((state->key_state[i] == 0) && (state->key_state[i] == 1))
                        {
                            state->waitingForKey = 0;
                            state->V[reg] = i;
                            state->programCounter += 2;
                            return;
                        }
                        state->save_key_state[i] = state->key_state[i];
                    }
                    return;
                }
            }
            break;
        case 0x15: state->delay = state->V[reg]; break;
        case 0x18: state->sound = state->V[reg]; break;
        case 0x1E: state->I += state->V[reg]; break;
        case 0x29: state->I = FONT_BASE + (state->V[reg] * 5); break;
        case 0x55:
            {
                //  store registers v0 through v0x in memory starting at lcoation i
                for (int i = 0; i <= reg; i++)
                {
                    state->memory[state->I + i] = state->V[i];
                }
                // set i to one after vx
                state->I += (reg + 1);
            }
            break;
        case 0x65:
            {
                
                for (int i = 0; i <= reg; i++)
                {
                    state->V[i] = state->memory[state->I + i];
                }
                state->I += (reg + 1);
            }
            break;
        default: unimplementedInstrunction(state); break;
    }
    state->programCounter += 2;
}