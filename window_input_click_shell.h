// File: window_input_click_shell.h
// Commit: Declare shell control click handling

#pragma once

#include "window_state.h"
#include "window_render.h"

bool processShellControlClick(
    const WindowRender::MapShellRects& shellRects,
    int mouseX,
    int mouseY,
    WindowLoopState& windowState
);