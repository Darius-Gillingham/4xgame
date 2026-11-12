// File: menu.h
// Commit: Keep menu settings focused on map size generation

#pragma once

#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdint>

enum class MenuAction
{
    none,
    continue_game,
    load_game,
    new_game,
    exit_game
};

struct MenuSettings
{
    int mapWidth = 160;
    int mapHeight = 96;
    bool useRandomSeed = true;
    std::uint32_t seed = 1337u;
    PlayerSetup playerSetup;
    bool confirmed = false;
    MenuAction action = MenuAction::none;
};

bool runMenu(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* sharedFont, MenuSettings& settings);