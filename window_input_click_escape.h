// File: window_input_click_escape.h
// Commit: Declare escape menu click handling

#pragma once

#include "game.h"
#include "map.h"
#include "window_render.h"
#include "window_state.h"

void processEscapeMenuClick(
    const Map& map,
    GameState& gameState,
    const WindowRender::MapShellRects& shellRects,
    int mouseX,
    int mouseY,
    WindowLoopState& windowState
);
