// File: window_helpers.h
// Commit: Pass province overlay scroll offset through holding geometry helper declarations

#pragma once

#include "building/province_screen_types.h"
#include "game.h"
#include "map.h"
#include "window_render.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <vector>

constexpr const char* kDefaultSavePath = "save_game.txt";
constexpr Uint32 kBaseMillisecondsPerGameMonth = 2500;

int wrapTileX(
    int x,
    int width
);

Uint32 monthIntervalForSpeed(
    int gameSpeed
);

bool selectedProvinceOwnedByPlayer(
    const GameState& gameState,
    int selectedProvinceId
);

SDL_Rect dynamicAddBuildingButtonRect(
    const SDL_Rect& overlayRect,
    const Province& province,
    TTF_Font* bodyFont,
    int scrollOffset
);

bool pointInAnyForegroundPanel(
    int mouseX,
    int mouseY,
    const WindowRender::MapShellRects& rects,
    const ProvinceOverlayState& provinceOverlay,
    const SDL_Rect& provinceOverlayRect
);

std::vector<ClickTarget> collectProvinceBuildingClickTargets(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SDL_Rect& overlayRect,
    const Province& province,
    int scrollOffset
);