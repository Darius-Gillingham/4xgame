// File: window_input_click_shell.cpp
// Commit: Extract shell control click handling from window.cpp

#include "window_input_click_shell.h"

#include <SDL2/SDL.h>

bool processShellControlClick(
    const WindowRender::MapShellRects& shellRects,
    int mouseX,
    int mouseY,
    WindowLoopState& windowState)
{
    if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.pauseButton
        )
    )
    {
        windowState.shell.gameSpeed = 0;
        windowState.lastMonthAdvanceMs = SDL_GetTicks();
        windowState.shell.monthProgress = 0;
        return true;
    }

    if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.speedOneButton
        )
    )
    {
        windowState.shell.gameSpeed = 1;
        windowState.lastMonthAdvanceMs = SDL_GetTicks();
        windowState.shell.monthProgress = 0;
        return true;
    }

    if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.speedTwoButton
        )
    )
    {
        windowState.shell.gameSpeed = 2;
        windowState.lastMonthAdvanceMs = SDL_GetTicks();
        windowState.shell.monthProgress = 0;
        return true;
    }

    if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.speedThreeButton
        )
    )
    {
        windowState.shell.gameSpeed = 3;
        windowState.lastMonthAdvanceMs = SDL_GetTicks();
        windowState.shell.monthProgress = 0;
        return true;
    }

    if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.speedFourButton
        )
    )
    {
        windowState.shell.gameSpeed = 4;
        windowState.lastMonthAdvanceMs = SDL_GetTicks();
        windowState.shell.monthProgress = 0;
        return true;
    }

    if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.speedFiveButton
        )
    )
    {
        windowState.shell.gameSpeed = 5;
        windowState.lastMonthAdvanceMs = SDL_GetTicks();
        windowState.shell.monthProgress = 0;
        return true;
    }

    if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.terrainModeButton
        )
    )
    {
        windowState.shell.activeMapMode = MapViewMode::terrain;
        return true;
    }

    if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.regionsModeButton
        )
    )
    {
        windowState.shell.activeMapMode = MapViewMode::regions;
        return true;
    }

    if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.provincesModeButton
        )
    )
    {
        windowState.shell.activeMapMode = MapViewMode::provinces;
        return true;
    }

    return false;
}