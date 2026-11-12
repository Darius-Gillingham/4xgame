// File: building/province_screen_capital_render.h
// Commit: Declare non-rendering holding-tree content height calculations for scroll clamping

#pragma once

#include "province.h"
#include "province_screen_types.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <vector>

int holdingTreeContentHeight(
    const Province& province,
    int holdingId,
    TTF_Font* bodyFont
);

int holdingRootBlockHeight(
    const Province& province,
    int holdingId,
    TTF_Font* bodyFont
);

void drawAddRowButton(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SDL_Rect& rect,
    bool active
);

void drawSlotRow(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SDL_Rect& rect,
    const std::string& label
);

int renderHoldingTree(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Province& province,
    int holdingId,
    HoldingType rootType,
    int startX,
    int startY,
    int width,
    int depth,
    std::vector<ClickTarget>& clickTargets
);

int renderHoldingRootBlock(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Province& province,
    int holdingId,
    const SDL_Rect& sectionRect,
    int startY,
    std::vector<ClickTarget>& clickTargets
);