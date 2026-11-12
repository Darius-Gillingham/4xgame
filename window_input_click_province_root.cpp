// File: window_input_click_province_root.cpp
// Commit: Add Army tab switching to the live province overlay

#include "window_input_click_province.h"

#include "window_helpers.h"

#include <string>

namespace
{
    bool processProvinceRootInteraction(
        GameState& gameState,
        const SDL_Rect& overlayRect,
        int mouseX,
        int mouseY,
        WindowLoopState& windowState
    )
    {
        Province* selectedProvince =
            provinceById(
                gameState,
                windowState.selectedProvinceId
            );

        if (!selectedProvince)
        {
            windowState.shell.statusText =
                "Province state not found.";

            return true;
        }

        const bool ownedByPlayer =
            selectedProvinceOwnedByPlayer(
                gameState,
                windowState.selectedProvinceId
            );

        const SDL_Rect holdingsTab =
            WindowRender::provinceBuildingsTabRect(
                overlayRect
            );

        const SDL_Rect populationTab =
            WindowRender::provincePopulationTabRect(
                overlayRect
            );

        const SDL_Rect armyTab =
            WindowRender::provinceArmyTabRect(
                overlayRect
            );

        const SDL_Rect sacredSiteCard =
            WindowRender::provinceSacredSiteCardRect(
                overlayRect
            );

        if (
            WindowRender::pointInRect(
                mouseX,
                mouseY,
                holdingsTab
            )
        )
        {
            windowState.provinceOverlay.activeTab =
                ProvinceOverlayTab::holdings;

            windowState.provinceOverlay.scrollOffset = 0;
            windowState.selectingRootBuilding = false;
            windowState.selectingSubBuildingParentId = -1;

            return true;
        }

        if (
            WindowRender::pointInRect(
                mouseX,
                mouseY,
                populationTab
            )
        )
        {
            windowState.provinceOverlay.activeTab =
                ProvinceOverlayTab::population;

            windowState.provinceOverlay.scrollOffset = 0;
            windowState.selectingRootBuilding = false;
            windowState.selectingSubBuildingParentId = -1;

            return true;
        }

        if (
            WindowRender::pointInRect(
                mouseX,
                mouseY,
                armyTab
            )
        )
        {
            windowState.provinceOverlay.activeTab =
                ProvinceOverlayTab::army;

            windowState.provinceOverlay.scrollOffset = 0;
            windowState.selectingRootBuilding = false;
            windowState.selectingSubBuildingParentId = -1;

            return true;
        }

        if (
            windowState.selectingRootBuilding &&
            WindowRender::pointInRect(
                mouseX,
                mouseY,
                sacredSiteCard
            )
        )
        {
            if (!ownedByPlayer)
            {
                windowState.shell.statusText =
                    "Cannot build in an unowned province.";

                windowState.selectingRootBuilding = false;

                return true;
            }

            std::string reason;

            if (
                selectedProvince->buildRootHolding(
                    HoldingType::sacred_site,
                    reason
                )
            )
            {
                windowState.provinceOverlay.scrollOffset = 0;
                windowState.selectingRootBuilding = false;
            }

            windowState.shell.statusText = reason;

            return true;
        }

        return false;
    }
}

bool processProvinceOverlayClick(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Map& map,
    GameState& gameState,
    const WindowRender::MapShellRects& shellRects,
    const SDL_Rect& overlayRect,
    int mouseX,
    int mouseY,
    WindowLoopState& windowState
)
{
    if (!windowState.provinceOverlay.open)
    {
        return false;
    }

    if (
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            shellRects.provinceOverlayCloseButton
        )
    )
    {
        windowState.provinceOverlay.open = false;
        windowState.provinceOverlay.scrollOffset = 0;
        windowState.selectingRootBuilding = false;
        windowState.selectingSubBuildingParentId = -1;
        windowState.shell.statusText =
            "Province window closed.";

        return true;
    }

    if (
        !WindowRender::pointInRect(
            mouseX,
            mouseY,
            overlayRect
        )
    )
    {
        return false;
    }

    if (
        processProvinceRootInteraction(
            gameState,
            overlayRect,
            mouseX,
            mouseY,
            windowState
        )
    )
    {
        return true;
    }

    return processProvinceBuildingInteraction(
        renderer,
        bodyFont,
        map,
        gameState,
        overlayRect,
        mouseX,
        mouseY,
        windowState
    );
}