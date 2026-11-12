// File: window_input_click_escape.cpp
// Commit: Extract escape menu click handling from window.cpp

#include "window_input_click_escape.h"

#include "save_game.h"
#include "window_helpers.h"

#include <string>

void processEscapeMenuClick(
    const Map& map,
    GameState& gameState,
    const WindowRender::MapShellRects& shellRects,
    int mouseX,
    int mouseY,
    WindowLoopState& windowState)
{
    if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.escapeCloseButton
        )
    )
    {
        windowState.shell.escapeMenuOpen = false;
    }
    else if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.escapeSaveButton
        )
    )
    {
        std::string reason;

        saveGameToFile(
            kDefaultSavePath,
            map,
            gameState,
            reason
        );

        windowState.shell.statusText = reason;
    }
    else if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.escapeLoadButton
        )
    )
    {
        windowState.shell.statusText =
            "Returned to main menu for loading.";

        windowState.running = false;
    }
    else if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.escapeExitButton
        )
    )
    {
        windowState.running = false;
    }
    else if (
        !WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.escapeMenu
        )
    )
    {
        windowState.shell.escapeMenuOpen = false;
    }
}