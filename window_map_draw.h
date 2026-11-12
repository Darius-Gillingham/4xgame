// File: window_map_draw.h
// Commit: Declare map rendering with persistent on-map unit support

#pragma once

#include "game.h"
#include "map.h"
#include "window_state.h"

#include <SDL2/SDL.h>

void drawWindowMap(
    SDL_Renderer* renderer,
    const Map& map,
    const GameState& gameState,
    const WindowLoopState& windowState
);