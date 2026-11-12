// File: window_update.cpp
// Commit: Synchronize raised levy units whenever the window simulation updates

#include "window_update.h"

#include "window_camera.h"
#include "window_helpers.h"

#include <SDL2/SDL.h>

#include <algorithm>
#include <string>

void updateWindowSimulation(
    const Map& map,
    GameState& gameState,
    WindowLoopState& windowState)
{
    synchronizeRaisedLevyUnits(
        map,
        gameState
    );

    const Uint32 currentMs = SDL_GetTicks();
    const Uint32 monthInterval =
        monthIntervalForSpeed(windowState.shell.gameSpeed);

    if (windowState.shell.gameSpeed > 0)
    {
        while (
            currentMs - windowState.lastMonthAdvanceMs >=
            monthInterval
        )
        {
            tickGame(gameState);

            synchronizeRaisedLevyUnits(
                map,
                gameState
            );

            windowState.lastMonthAdvanceMs += monthInterval;

            windowState.shell.statusText =
                std::string("Advanced to ") +
                monthName(gameState.date.month) +
                " Year " +
                std::to_string(gameState.date.year) +
                ".";
        }

        windowState.shell.monthProgress =
            static_cast<int>(
                (
                    std::min<Uint32>(
                        currentMs - windowState.lastMonthAdvanceMs,
                        monthInterval
                    ) *
                    static_cast<Uint32>(
                        windowState.shell.monthProgressTarget
                    )
                ) /
                monthInterval
            );
    }
    else
    {
        windowState.lastMonthAdvanceMs = currentMs;
        windowState.shell.monthProgress = 0;
    }
}

void updateWindowCamera(
    const Map& map,
    const WindowRender::MapShellRects& shellRects,
    const SDL_Rect& overlayRect,
    int mapPanelHeight,
    int edgeScrollMargin,
    float keyboardScrollSpeed,
    float edgeScrollSpeed,
    WindowLoopState& windowState)
{
    const Uint8* keys = SDL_GetKeyboardState(nullptr);

    if (!windowState.shell.escapeMenuOpen)
    {
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])
        {
            windowState.camera.offsetY += keyboardScrollSpeed;
        }

        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])
        {
            windowState.camera.offsetY -= keyboardScrollSpeed;
        }

        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
        {
            windowState.camera.offsetX += keyboardScrollSpeed;
        }

        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
        {
            windowState.camera.offsetX -= keyboardScrollSpeed;
        }
    }

    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (
        !windowState.shell.escapeMenuOpen &&
        !pointInAnyForegroundPanel(
            mouseX,
            mouseY,
            shellRects,
            windowState.provinceOverlay,
            overlayRect
        )
    )
    {
        if (mouseX <= edgeScrollMargin)
        {
            windowState.camera.offsetX += edgeScrollSpeed;
        }
        else if (
            mouseX >=
            windowState.screenWidth - edgeScrollMargin
        )
        {
            windowState.camera.offsetX -= edgeScrollSpeed;
        }

        if (mouseY <= edgeScrollMargin)
        {
            windowState.camera.offsetY += edgeScrollSpeed;
        }
        else if (
            mouseY >=
            windowState.screenHeight - edgeScrollMargin
        )
        {
            windowState.camera.offsetY -= edgeScrollSpeed;
        }
    }

    clampCamera(
        windowState.camera,
        windowState.screenWidth,
        windowState.screenHeight,
        mapPanelHeight,
        map
    );

    windowState.hoveredTileX = -1;
    windowState.hoveredTileY = -1;

    screenToTile(
        mouseX,
        mouseY,
        mapPanelHeight,
        windowState.camera,
        map,
        windowState.hoveredTileX,
        windowState.hoveredTileY
    );
}