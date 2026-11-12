// File: window_helpers.cpp
// Commit: Apply province overlay scroll offset to holding geometry and click targets

#include "window_helpers.h"

#include "building/province_screen_capital_render.h"
#include "map_province_environment.h"

#include <algorithm>

int wrapTileX(
    int x,
    int width
)
{
    if (width <= 0)
    {
        return 0;
    }

    int wrapped = x % width;

    if (wrapped < 0)
    {
        wrapped += width;
    }

    return wrapped;
}

Uint32 monthIntervalForSpeed(
    int gameSpeed
)
{
    if (gameSpeed <= 0)
    {
        return kBaseMillisecondsPerGameMonth;
    }

    return std::max<Uint32>(
        250,
        kBaseMillisecondsPerGameMonth /
        static_cast<Uint32>(gameSpeed)
    );
}

bool selectedProvinceOwnedByPlayer(
    const GameState& gameState,
    int selectedProvinceId
)
{
    const ProvinceSlot* provinceSlot =
        provinceSlotById(
            gameState,
            selectedProvinceId
        );

    if (!provinceSlot)
    {
        return false;
    }

    return provinceSlot->countryId ==
           gameState.playerCounty.countryId;
}

SDL_Rect dynamicAddBuildingButtonRect(
    const SDL_Rect& overlayRect,
    const Province& province,
    TTF_Font* bodyFont,
    int scrollOffset
)
{
    const int contentTop = overlayRect.y + 92;
    const int contentHeight = overlayRect.h - 106;

    SDL_Rect holdingPanel{
        overlayRect.x + 14,
        contentTop,
        (overlayRect.w * 58) / 100,
        contentHeight
    };

    int y =
        holdingPanel.y +
        44 -
        scrollOffset;

    for (int rootId : province.rootHoldingIds())
    {
        y +=
            holdingRootBlockHeight(
                province,
                rootId,
                bodyFont
            );

        y += 14;
    }

    return WindowRender::provinceAddBuildingButtonRectAtY(
        overlayRect,
        y
    );
}

bool pointInAnyForegroundPanel(
    int mouseX,
    int mouseY,
    const WindowRender::MapShellRects& rects,
    const ProvinceOverlayState& provinceOverlay,
    const SDL_Rect& provinceOverlayRect
)
{
    if (WindowRender::pointInRect(
            mouseX,
            mouseY,
            rects.topResourceBar))
    {
        return true;
    }

    if (WindowRender::pointInRect(
            mouseX,
            mouseY,
            rects.speedPanel))
    {
        return true;
    }

    if (WindowRender::pointInRect(
            mouseX,
            mouseY,
            rects.mapModePanel))
    {
        return true;
    }

    if (
        provinceOverlay.open &&
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            provinceOverlayRect
        )
    )
    {
        return true;
    }

    return false;
}

std::vector<ClickTarget> collectProvinceBuildingClickTargets(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SDL_Rect& overlayRect,
    const Province& province,
    int scrollOffset
)
{
    std::vector<ClickTarget> clickTargets;

    const int contentTop = overlayRect.y + 92;
    const int contentHeight = overlayRect.h - 106;

    SDL_Rect holdingPanel{
        overlayRect.x + 14,
        contentTop,
        (overlayRect.w * 58) / 100,
        contentHeight
    };

    int y =
        holdingPanel.y +
        44 -
        scrollOffset;

    for (int rootId : province.rootHoldingIds())
    {
        y += renderHoldingRootBlock(
            renderer,
            bodyFont,
            province,
            rootId,
            holdingPanel,
            y,
            clickTargets
        );

        y += 14;
    }

    return clickTargets;
}