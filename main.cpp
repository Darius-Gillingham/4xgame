// File: main.cpp
// Commit: Fix SDL entry point signature for shared window app flow

#include "menu.h"
#include "window.h"
#include "map.h"
#include "map_internal.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <array>
#include <iostream>
#include <string>

namespace
{
    TTF_Font* loadFont(int size)
    {
        const std::array<const char*, 5> paths = {
            "C:/Windows/Fonts/georgia.ttf",
            "C:/Windows/Fonts/times.ttf",
            "C:/Windows/Fonts/timesbd.ttf",
            "C:/Windows/Fonts/arial.ttf",
            "C:/Windows/Fonts/calibri.ttf"
        };

        for (const char* path : paths)
        {
            if (TTF_Font* font = TTF_OpenFont(path, size))
            {
                return font;
            }
        }

        return nullptr;
    }

    void drawCenteredText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int screenWidth, int screenHeight)
    {
        if (!renderer || !font)
        {
            return;
        }

        SDL_Color color{0, 0, 0, 255};
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
        if (!surface)
        {
            return;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture)
        {
            SDL_FreeSurface(surface);
            return;
        }

        SDL_Rect dst{
            (screenWidth - surface->w) / 2,
            (screenHeight - surface->h) / 2,
            surface->w,
            surface->h
        };

        SDL_FreeSurface(surface);
        SDL_RenderCopy(renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }

    void drawLoadingScreen(SDL_Renderer* renderer, TTF_Font* font)
    {
        int screenWidth = 0;
        int screenHeight = 0;
        SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

        SDL_SetRenderDrawColor(renderer, 239, 226, 198, 255);
        SDL_RenderClear(renderer);
        drawCenteredText(renderer, font, "Generating Map...", screenWidth, screenHeight);
        SDL_RenderPresent(renderer);
    }
}

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
        "4X Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        900,
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

    TTF_Font* sharedFont = loadFont(22);

    bool appRunning = true;

    while (appRunning)
    {
        MenuSettings settings;

        if (!runMenu(window, renderer, sharedFont, settings) || !settings.confirmed)
        {
            break;
        }

        drawLoadingScreen(renderer, sharedFont);
        SDL_Delay(50);

        Map map(settings.mapWidth, settings.mapHeight, settings.seed);
        map.generate();

        MapState state = buildRegionLayout(map, settings.seed);
        applyLandmask(map, state);
        assignTerrainClusters(map, settings.seed);
        smoothTerrainPasses(map);

        runWindow(window, renderer, sharedFont, map, state);
        appRunning = false;
    }

    if (sharedFont)
    {
        TTF_CloseFont(sharedFont);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}