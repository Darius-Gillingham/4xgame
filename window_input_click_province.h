// File: window_input_click_province.h
// Commit: Update province overlay declarations to holding and building terminology

#pragma once

#include "game.h"
#include "map.h"
#include "window_render.h"
#include "window_state.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

bool processProvinceOverlayClick(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Map& map,
    GameState& gameState,
    const WindowRender::MapShellRects& shellRects,
    const SDL_Rect& overlayRect,
    int mouseX,
    int mouseY,
    WindowLoopState& windowState
);

bool processProvinceBuildingInteraction(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Map& map,
    GameState& gameState,
    const SDL_Rect& overlayRect,
    int mouseX,
    int mouseY,
    WindowLoopState& windowState
);