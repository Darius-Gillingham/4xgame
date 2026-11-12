// File: window.h
// Commit: Use extracted camera module for map window camera state

#pragma once

#include "map.h"
#include "map_internal.h"
#include "game.h"
#include "window_camera.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void runWindow(
    SDL_Window* window,
    SDL_Renderer* renderer,
    TTF_Font* sharedFont,
    const Map& map,
    const MapState& state,
    GameState& gameState
);