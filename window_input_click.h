// File: window_input_click.h
// Commit: Declare extracted window left-click handling

#pragma once

#include "game.h"
#include "map.h"
#include "window_render.h"
#include "window_state.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void processWindowLeftClick(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Map& map,
    GameState& gameState,
    const WindowRender::MapShellRects& shellRects,
    const SDL_Rect& overlayRect,
    int mapPanelHeight,
    int mouseX,
    int mouseY,
    WindowLoopState& windowState
);