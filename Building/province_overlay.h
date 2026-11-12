// File: building/province_overlay.h
// Commit: Rename province overlay holdings tab declaration while preserving overlay behavior

#pragma once

#include "province.h"
#include "province_screen_types.h"
#include "../game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

SDL_Rect provinceOverlayRect(
    int screenWidth,
    int screenHeight,
    int topPanelHeight
);

SDL_Rect provinceOverlayHoldingsTabRect(
    const SDL_Rect& overlayRect
);

SDL_Rect provinceOverlayPopulationTabRect(
    const SDL_Rect& overlayRect
);

SDL_Rect provinceOverlayCloseRect(
    const SDL_Rect& overlayRect
);

bool handleProvinceOverlayClick(
    GameState& gameState,
    ProvinceOverlayState& overlay,
    const SDL_Rect& overlayRect,
    int mouseX,
    int mouseY
);

void drawProvinceOverlay(
    SDL_Renderer* renderer,
    TTF_Font* titleFont,
    TTF_Font* bodyFont,
    const GameState& gameState,
    const ProvinceOverlayState& overlay,
    const SDL_Rect& overlayRect
);