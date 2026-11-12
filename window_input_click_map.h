// File: window_input_click_map.h
// Commit: Declare map province selection click handling

#pragma once

#include "map.h"
#include "window_render.h"
#include "window_state.h"

void processMapSelectionClick(
    const Map& map,
    const WindowRender::MapShellRects& shellRects,
    const SDL_Rect& overlayRect,
    int mapPanelHeight,
    int mouseX,
    int mouseY,
    WindowLoopState& windowState
);