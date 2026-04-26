// File: main.cpp
// Commit: Launch directly into the scrollable province shell UI with shared SDL window setup

#include "province.h"
#include "province_screen.h"
#include "ui.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    if (TTF_Init() != 0)
    {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Province Builder",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1500,
        960,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
    );

    if (!window)
    {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer)
    {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* sharedFont = loadFontOrNull(18);
    if (!sharedFont)
    {
        std::cerr << "Failed to load a font from C:/Windows/Fonts.\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Province province;
    runProvinceScreen(window, renderer, sharedFont, province);

    TTF_CloseFont(sharedFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}