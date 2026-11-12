// File: window_input.h
// Commit: Declare extracted window event processing

#pragma once

#include "game.h"
#include "map.h"
#include "window_render.h"
#include "window_state.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void processWindowEvents(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Map& map,
    GameState& gameState,
    const WindowRender::MapShellRects& shellRects,
    const SDL_Rect& overlayRect,
    int mapPanelHeight,
    WindowLoopState& windowState
);