// File: window_input_click_map.cpp
// Commit: Initialize the province overlay on the Holdings tab only

#include "window_input_click_map.h"

#include "window_camera.h"
#include "window_helpers.h"

void processMapSelectionClick(
    const Map& map,
    const WindowRender::MapShellRects& shellRects,
    const SDL_Rect& overlayRect,
    int mapPanelHeight,
    int mouseX,
    int mouseY,
    WindowLoopState& windowState)
{
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

    int tileX = -1;
    int tileY = -1;

    if (
        !screenToTile(
            mouseX,
            mouseY,
            mapPanelHeight,
            windowState.camera,
            map,
            tileX,
            tileY
        )
    )
    {
        return;
    }

    const int clickedProvinceId =
        map.at(
            tileX,
            tileY
        ).provinceId;

    if (clickedProvinceId < 0)
    {
        windowState.shell.statusText =
            "No province exists on this tile.";
        return;
    }

    windowState.selectedTileX = tileX;
    windowState.selectedTileY = tileY;
    windowState.selectedProvinceId = clickedProvinceId;

    windowState.provinceOverlay.open = true;
    windowState.provinceOverlay.activeTab =
        ProvinceOverlayTab::holdings;
    windowState.provinceOverlay.scrollOffset = 0;

    windowState.selectingRootBuilding = false;
    windowState.selectingSubBuildingParentId = -1;

    windowState.shell.statusText =
        "Province window opened.";
}