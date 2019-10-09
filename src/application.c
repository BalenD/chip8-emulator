#include <stdio.h>
#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h> 
#include <SDL2/SDL_timer.h> 

int main(int argc, char**argv)
{
    if ()
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