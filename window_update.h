// File: window_update.h
// Commit: Pass the map into simulation updates for levy unit synchronization

#pragma once

#include "game.h"
#include "map.h"
#include "window_render.h"
#include "window_state.h"

void updateWindowSimulation(
    const Map& map,
    GameState& gameState,
    WindowLoopState& windowState
);

void updateWindowCamera(
    const Map& map,
    const WindowRender::MapShellRects& shellRects,
    const SDL_Rect& overlayRect,
    int mapPanelHeight,
    int edgeScrollMargin,
    float keyboardScrollSpeed,
    float edgeScrollSpeed,
    WindowLoopState& windowState
);