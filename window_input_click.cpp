// File: window_input_click.cpp
// Commit: Dispatch window left-click handling by interface concern

#include "window_input_click.h"

#include "window_input_click_escape.h"
#include "window_input_click_map.h"
#include "window_input_click_province.h"
#include "window_input_click_shell.h"

void processWindowLeftClick(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Map& map,
    GameState& gameState,
    const WindowRender::MapShellRects& shellRects,
    const SDL_Rect& overlayRect,
    int mapPanelHeight,
    int mouseX,
    int mouseY,
    WindowLoopState& windowState)
{
    if (windowState.shell.escapeMenuOpen)
    {
        processEscapeMenuClick(
            map,
            gameState,
            shellRects,
            mouseX,
            mouseY,
            windowState
        );

        return;
    }

    if (
        processShellControlClick(
            shellRects,
            mouseX,
            mouseY,
            windowState
        )
    )
    {
        return;
    }

    if (
        processProvinceOverlayClick(
            renderer,
            bodyFont,
            map,
            gameState,
            shellRects,
            overlayRect,
            mouseX,
            mouseY,
            windowState
        )
    )
    {
        return;
    }

    processMapSelectionClick(
        map,
        shellRects,
        overlayRect,
        mapPanelHeight,
        mouseX,
        mouseY,
        windowState
    );
}