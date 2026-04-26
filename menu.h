// File: menu.h
// Commit: Refactor menu interface to render inside the shared app window and renderer

#pragma once

#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdint>

struct MenuSettings
{
    int mapWidth = 160;
    int mapHeight = 96;
    int tileSize = 8;
    bool useRandomSeed = true;
    std::uint32_t seed = 1337u;
    PlayerSetup playerSetup;
    bool confirmed = false;
};

bool runMenu(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* sharedFont, MenuSettings& settings);