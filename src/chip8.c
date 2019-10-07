#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"
#include "font.h"
#include "opcodes.h"


static void Disassembler();

int main(int argc, char**argv)
{
    FILE *f = fopen(argv[1], "rb");

    if (f == NULL)
    {
        printf("error: Couldnt open %s\n", argv[1]);
        exit(1);
    }

    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    unsigned char *buffer = malloc(fsize + 0x200);
    fread(buffer + 0x200, fsize, 1, f);
    fclose(f);

    int pc = 0x200;

    while (pc < (fsize + 0x200))
    {
        Disassembler(buffer, pc);
        pc += 2;
        printf("\n");
    }

    return 0;
}

static void Disassembler(uint8_t *codeBuffer, int pc)
{
    
    // store address of codeBuffer in pointer variable
    uint8_t *code = &codeBuffer[pc];

    // shift the first byte by 4 places
    // this is done because its stored big-endean, so we move it
    // Example: 1111 0000 > 0000 1111
    uint8_t firstNib = (code[0] >> 4);

    // the x means it will print an int in hex
    // 4 means will be printed left justified with at least four  digits
    //  0 means if less than 4 digits, print leading  zeros 
    printf("%04x %02x %02x", pc, code[0], code[1]);

    switch (firstNib)
    {
        case 0x0:
            {
                switch (code[1])
                {
                    case 0xE0: printf("%10s %-10s", "Display", "Clear Screen"); break;
                    case 0xEE: printf("%10s %-10s", "Flow", "Return from subroutine"); break;
                    default:  printf("%10s 0", "Unknown"); break;
                    
                }
                break;
            }
        case 0x1: printf("%7s %-10s $%01x%02x","Flow", "JUMP", code[0]&0xf, code[1]); break;
        case 0x2: printf("%7s %-10s $%01x%02x","Flow", "CALL", code[0]&0xf, code[1]); break;
        case 0x3: printf("%7s %-10s V%01X,#$%02x","Cond", "SKIP EQUAL", code[0]&0xf, code[1]); break;
        case 0x4: printf("%7s %-10s V%01X,#$%02x","Cond", "SKIP !EQUAL", code[0]&0xf, code[1]); break;
        case 0x5: printf("%7s %-10s V%01X,V%01X","Cond", "SKIP EQUAL", code[0]&0xf, code[1]); break;
        case 0x6: printf("%7s %-10s V%01X,#$%02x","Const", "SET", code[0]&0xf, code[1]); break;
        case 0x7: printf("%7s %-10s V%01X,#$%02x","Const", "ADD", code[0]&0xf, code[1]); break;
        case 0x8:
            {
                uint8_t lastBit = code[1] & 0xf;
                switch (lastBit)
                {
                    case 0: printf("%7s %-10s V%01x, V%01x","Const", "SET", code[0] & 0xf, code[1] >> 4); break;
                    case 1: printf("%7s %-10s V%01x, V%01x","BitOp", "OR", code[0] & 0xf, code[1] >> 4); break;
                    case 2: printf("%7s %-10s V%01x, V%01x","BitOp", "AND", code[0] & 0xf, code[1] >> 4); break;
                    case 3: printf("%7s %-10s V%01x, V%01x","BitOp", "XOR", code[0] & 0xf, code[1] >> 4); break;
                    case 4: printf("%7s %-10s V%01x, V%01x","Math", "Add", code[0] & 0xf, code[1] >> 4); break;
                    case 5: printf("%7s %-10s V%01x, V%01x, V%01x","Math", "SUB", code[0] & 0xf, code[0]&0xf, code[1] >> 4); break;
                    case 6: printf("%7s %-10s V%01x, V%01x","BitOp", "SHR", code[0] & 0xf, code[1] >> 4); break;
                    case 7: printf("%7s %-10s V%01x, V%01x, V%01x","Math", "SUB", code[0] & 0xf, code[1] >> 4, code[1] >> 4); break;
                    case 0xE: printf("%7s %-10s V%01x, V%01x","BitOp", "SHL", code[0] & 0xf, code[1] >> 4); break;
                    default: printf("%10s 8", "UNKNOWN"); break;

                }
            }
            break;
        case 0x9: printf("%7s %-10s V%01x, V%01x","Cond", "SKIP !EQUAL", code[0]&0xf, code[1] >> 4); break;
        case 0xA: printf("%7s %-10s I, #$%01x%02x","Mem", "sET I", code[0]&0xf, code[1]); break;
        case 0xB: printf("%7s %-10s $%01x%02x(V0)","Flow", "JUMP", code[0]&0xf, code[1]); break;
        case 0xC: printf("%7s %-10s V%01x, #$%02x","RAND", "RNDMASK", code[0]&0xf, code[1]); break;
        case 0xD: printf("%7s %-10s V%01x, V%01x, #$%01x","Display", "DRAW", code[0] & 0xf, code[1] >> 4, code[1] & 0xf); break;
        case 0xE:
            {
                switch (code[1])
                {
                    case 0x9E: printf("%7s %-10s V%01x","KeyOp", "SKIP KEY Y", code[0] & 0xf); break;
                    case 0xA1: printf("%7s %-10s V%01x","KeyOp", "SKIP KEY N", code[0] & 0xf); break;
                    default: printf("%10s E", "UNKNOWN"); break;
                }
            }
            break;
        case 0xF:
            {
                switch (code[1])
                {
                    case 0x07: printf("%7s %-10s V%01x, DELAY","Timer", "GET DELAY", code[0] & 0xf); break;
                    case 0x0A: printf("%7s %-10s V%01x","KeyOp", "GET KEY", code[0] & 0xf); break;
                    case 0x15: printf("%7s %-10s DLEAY, V%01x","Timer", "SET DELAY", code[0] & 0xf); break;
                    case 0x18: printf("%7s %-10s SOUND, V%01x","Sound", "SET SOUND", code[0] & 0xf); break;
                    case 0x1E: printf("%7s %-10s I, V%01x","Mem", "ADD TO I", code[0] & 0xf); break;
                    case 0x29: printf("%7s %-10s I, V%01x","Mem", "SPRITECHAR", code[0] & 0xf); break;
                    case 0x33: printf("%7s %-10s (I), V%01x","Mem", "MOVBCD", code[0] & 0xf); break;
                    case 0x55: printf("%7s %-10s (I), V0-V%01X","Mem", "MOVM", code[0] & 0xf); break;
                    case 0x65: printf("%7s %-10s V0-V%01X, (I)","Mem", "MOVM", code[0] & 0xf); break;
                    default: printf("%10s F", "UNKNOWN"); break;
                }
            }
            break;

    }

}

Chip8State* InitChip8(void)
{
    // allocate memory of chip 8 state size, and only 1 of it
    // return a pointer to the memory
    Chip8State* s = calloc(sizeof(Chip8State), 1);
    // allocate 4096 RAM memory
    s->memory = calloc(1024 * 4, 1);
    // the screen buffer starts at 3840
    s->screen = &s->memory[0xF00];
    // program counter starts at 512
    s->programCounter = 0x200;

    // copy the font over
    // the font takes from 0 to 80 of the memory
    memcpy(&s->memory[FONT_BASE], font, FONT_SIZE);

    return s;
}

static void Emulate(Chip8State *state)
{
    // find the address of the current executing instruction in memory
    // NOTE: an instruction is 2 bytes, retrieve the whole instruction
    uint16_t instruction = (state->memory[state->programCounter] << 8) | state->memory[state->programCounter +1];

    Disassembler(state->memory, state->programCounter);

    // get the highNib to find out which instruction catagory it is
    /* 
       NOTE:
       since its stored big endean, bitwise AND it by 1111 0000 0000 0000
       and shift by 4 to get instruction to the far right
    */
    int highNib = (instruction & 0xf0) >> 12;

    switch (highNib)
    {
        case 0x00: OpCode0(state, instruction); break;
        case 0x01: OpCode1(state, instruction); break;
        case 0x02: OpCode2(state, instruction); break;
        case 0x03: OpCode3(state, instruction); break;
        case 0x04: Opcode4(state, instruction); break;
        case 0x05: OpCode5(state, instruction); break;
        case 0x06: OpCode6(state, instruction); break;
        case 0x07: OpCode7(state, instruction); break;
        case 0x08: OpCode8(state, instruction); break;
        case 0x09: OpCode9(state, instruction); break;
        case 0x0a: OpCodeA(state, instruction); break;
        case 0x0b: OpCodeB(state, instruction); break;  
        case 0x0c: opcodeC(state, instruction); break;
        case 0x0d: OpCodeD(state, instruction); break;
        case 0x0e: OpCodeE(state, instruction); break;
        case 0x0f: opcodeF(state, instruction); break;
    }
}

