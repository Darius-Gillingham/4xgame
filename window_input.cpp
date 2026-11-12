// File: window_input.cpp
// Commit: Clamp province overlay scrolling to the active tab content height

#include "window_input.h"

#include "window_camera.h"
#include "window_helpers.h"
#include "window_input_click.h"

#include <algorithm>

namespace
{
    void handleEscapeKey(WindowLoopState& windowState)
    {
        if (windowState.selectingRootBuilding)
        {
            windowState.selectingRootBuilding = false;
            windowState.shell.statusText = "Holding selection closed.";
        }
        else if (windowState.selectingSubBuildingParentId >= 0)
        {
            windowState.selectingSubBuildingParentId = -1;
            windowState.shell.statusText = "Building selection closed.";
        }
        else
        {
            windowState.shell.escapeMenuOpen =
                !windowState.shell.escapeMenuOpen;
        }
    }

    int provinceOverlayMaximumScroll(
        TTF_Font* bodyFont,
        const GameState& gameState,
        const Map& map,
        const SDL_Rect& overlayRect,
        const WindowLoopState& windowState
    )
    {
        const Province* selectedProvince =
            provinceById(
                gameState,
                windowState.selectedProvinceId
            );

        if (!selectedProvince)
        {
            return 0;
        }

        const int contentHeight =
            overlayRect.h - 106;

        int tabContentHeight = 0;

        if (
            windowState.provinceOverlay.activeTab ==
            ProvinceOverlayTab::holdings
        )
        {
            tabContentHeight =
                WindowRender::provinceHoldingsContentHeight(
                    bodyFont,
                    overlayRect,
                    *selectedProvince
                );
        }
        else if (
            windowState.provinceOverlay.activeTab ==
            ProvinceOverlayTab::population
        )
        {
            tabContentHeight =
                WindowRender::provincePopulationContentHeight(
                    bodyFont,
                    map,
                    windowState.selectedProvinceId,
                    *selectedProvince
                );
        }
        else if (
            windowState.provinceOverlay.activeTab ==
            ProvinceOverlayTab::army
        )
        {
            tabContentHeight =
                WindowRender::provinceArmyContentHeight(
                    bodyFont,
                    *selectedProvince
                );
        }

        return
            std::max(
                0,
                tabContentHeight -
                contentHeight
            );
    }

    void handleMouseWheel(
        const SDL_Event& event,
        TTF_Font* bodyFont,
        const Map& map,
        const GameState& gameState,
        const WindowRender::MapShellRects& shellRects,
        const SDL_Rect& overlayRect,
        int mapPanelHeight,
        WindowLoopState& windowState)
    {
        int mouseX = 0;
        int mouseY = 0;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (windowState.shell.escapeMenuOpen)
        {
            return;
        }

        if (
            windowState.provinceOverlay.open &&
            WindowRender::pointInRect(
                mouseX,
                mouseY,
                overlayRect
            )
        )
        {
            const int maximumScroll =
                provinceOverlayMaximumScroll(
                    bodyFont,
                    gameState,
                    map,
                    overlayRect,
                    windowState
                );

            windowState.provinceOverlay.scrollOffset =
                std::clamp(
                    windowState.provinceOverlay.scrollOffset -
                        (event.wheel.y * 32),
                    0,
                    maximumScroll
                );

            return;
        }

        if (
            pointInAnyForegroundPanel(
                mouseX,
                mouseY,
                shellRects,
                windowState.provinceOverlay,
                overlayRect
            )
        )
        {
            return;
        }

        if (event.wheel.y > 0)
        {
            applyZoom(
                windowState.camera,
                1.15f,
                mouseX,
                mouseY,
                mapPanelHeight,
                windowState.screenWidth,
                windowState.screenHeight,
                map
            );
        }
        else if (event.wheel.y < 0)
        {
            applyZoom(
                windowState.camera,
                1.0f / 1.15f,
                mouseX,
                mouseY,
                mapPanelHeight,
                windowState.screenWidth,
                windowState.screenHeight,
                map
            );
        }
    }
}

void processWindowEvents(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Map& map,
    GameState& gameState,
    const WindowRender::MapShellRects& shellRects,
    const SDL_Rect& overlayRect,
    int mapPanelHeight,
    WindowLoopState& windowState)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            windowState.running = false;
        }
        else if (
            event.type == SDL_KEYDOWN &&
            event.key.keysym.sym == SDLK_ESCAPE
        )
        {
            handleEscapeKey(windowState);
        }
        else if (
            event.type == SDL_MOUSEBUTTONDOWN &&
            event.button.button == SDL_BUTTON_LEFT
        )
        {
            processWindowLeftClick(
                renderer,
                bodyFont,
                map,
                gameState,
                shellRects,
                overlayRect,
                mapPanelHeight,
                event.button.x,
                event.button.y,
                windowState
            );
        }
        else if (event.type == SDL_MOUSEWHEEL)
        {
            handleMouseWheel(
                event,
                bodyFont,
                map,
                gameState,
                shellRects,
                overlayRect,
                mapPanelHeight,
                windowState
            );
        }
    }
}