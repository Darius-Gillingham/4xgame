// File: province_screen_capital_render.h
// Commit: Pass root building type through tree rendering so every node can inherit the root background color

#pragma once

#include "province.h"
#include "province_screen_types.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

void drawAddRowButton(SDL_Renderer* renderer, TTF_Font* bodyFont, const SDL_Rect& rect, bool active);
void drawSlotRow(SDL_Renderer* renderer, TTF_Font* bodyFont, const SDL_Rect& rect, const std::string& label);

int renderBuildingTree(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Province& province,
    int buildingId,
    BuildingType rootType,
    int startX,
    int startY,
    int width,
    int depth,
    std::vector<ClickTarget>& clickTargets
);

int renderBuildingRootBlock(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Province& province,
    int buildingId,
    const SDL_Rect& sectionRect,
    int startY,
    std::vector<ClickTarget>& clickTargets
);