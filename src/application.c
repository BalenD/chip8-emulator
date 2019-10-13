#include <stdio.h>
#include <SDL2/SDL.h> 
#include <stdlib.h>
#include <stdint.h>
#include "chip8.h"

uint8_t keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

int main(int argc, char** argv)
{
    printf("HELLO");
    if  (argc < 2)
    {
        printf("No input file\n");
    }

    Chip8State* chip = InitChip8();
    int windowWidth = 1024;
    int windowHeight =  512;

    SDL_Window* window = NULL;
    
    

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("SDL initiialization failed! SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow(
        "chip8 emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        windowWidth, windowHeight,
        SDL_WINDOW_SHOWN);
    
    if (window == NULL)
    {
        printf("Window creation failed");
        exit(2);
    }

    //uint32_t render_flags = SDL_RENDERER_ACCELERATED;

    SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (render == NULL)
    {
        printf("Render failed! SDL ERROR: %s\n", SDL_GetError());
        exit(2);
    }
    else
    {
        SDL_SetRenderDrawColor(render, 0xFF, 0xFF, 0xFF, 0xFF);
    }
    
    SDL_RenderSetLogicalSize(render, windowWidth, windowHeight);

    SDL_Texture* texture = SDL_CreateTexture(
        render, 
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        64, 32);

    uint32_t tempPixels[2048];

    loadGameToMemory(argv[1], chip);
    int close = 0;

    while (!close)
    {
        Emulate(chip);

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT: close = 1; break;
                case SDL_KEYDOWN:
                    {
                        if (e.key.keysym.sym == SDLK_ESCAPE)
                        {
                            close = 1;
                        }
                        
                        if (e.key.keysym.sym == SDLK_F1)
                        {
                            // go back to  rom selction
                        }

                        for (int i = 0; i < 16; i++)
                        {
                            if (e.key.keysym.sym == keymap[i])
                            {
                                chip->key_state[i] = 0;
                            }
                        }
                        
                    }
                    break;
            }
        }

        if (chip->drawing == 1)
        {
            chip->drawing = 0;

            for (size_t i = 0; i < 2048; i++)
            {
                Uint8 px = chip->screen[i];
                tempPixels[i] = (0x00FFFFFF * px) | 0xFF000000;
            }

            SDL_UpdateTexture(texture, NULL, tempPixels,  64 * sizeof(uint32_t));

            SDL_RenderClear(render);

            SDL_RenderCopy(render, texture, NULL, NULL);

            SDL_RenderPresent(render);
            
        }
    }

    exit(0);
}
