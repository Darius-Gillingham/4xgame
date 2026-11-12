// File: window_render_province_rects.cpp
// Commit: Add Army tab and scroll-aware levy row geometry

#include <SDL2/SDL.h>

#include <algorithm>

namespace WindowRender
{
    SDL_Rect provinceOverlayRect(
        int screenWidth,
        int screenHeight,
        int topPanelHeight
    )
    {
        const int width =
            screenWidth / 2;

        const int height =
            (screenHeight * 2) / 3;

        return SDL_Rect{
            12,
            screenHeight - height - 12,
            width,
            height
        };
    }

    SDL_Rect provinceBuildingsTabRect(
        const SDL_Rect& overlayRect
    )
    {
        return SDL_Rect{
            overlayRect.x + 12,
            overlayRect.y + 44,
            132,
            34
        };
    }

    SDL_Rect provincePopulationTabRect(
        const SDL_Rect& overlayRect
    )
    {
        return SDL_Rect{
            overlayRect.x + 152,
            overlayRect.y + 44,
            132,
            34
        };
    }

    SDL_Rect provinceArmyTabRect(
        const SDL_Rect& overlayRect
    )
    {
        return SDL_Rect{
            overlayRect.x + 292,
            overlayRect.y + 44,
            132,
            34
        };
    }

    SDL_Rect provinceAddBuildingButtonRect(
        const SDL_Rect& overlayRect
    )
    {
        const int contentTop =
            overlayRect.y + 92;

        SDL_Rect buildingPanel{
            overlayRect.x + 14,
            contentTop,
            (overlayRect.w * 58) / 100,
            overlayRect.h - 106
        };

        return SDL_Rect{
            buildingPanel.x + 12,
            buildingPanel.y + 44,
            buildingPanel.w - 24,
            42
        };
    }

    SDL_Rect provinceAddBuildingButtonRectAtY(
        const SDL_Rect& overlayRect,
        int y
    )
    {
        const int contentTop =
            overlayRect.y + 92;

        SDL_Rect buildingPanel{
            overlayRect.x + 14,
            contentTop,
            (overlayRect.w * 58) / 100,
            overlayRect.h - 106
        };

        return SDL_Rect{
            buildingPanel.x + 12,
            y,
            buildingPanel.w - 24,
            42
        };
    }

    SDL_Rect provinceSacredSiteCardRect(
        const SDL_Rect& overlayRect
    )
    {
        const int contentTop =
            overlayRect.y + 92;

        return SDL_Rect{
            overlayRect.x + 28,
            contentTop + 54,
            260,
            132
        };
    }

    SDL_Rect provinceSubBuildingCardRect(
        const SDL_Rect& overlayRect,
        int cardIndex
    )
    {
        const int contentTop =
            overlayRect.y + 92;

        const int cardWidth = 260;
        const int cardHeight = 132;
        const int cardGap = 14;

        const int contentWidth =
            overlayRect.w - 56;

        const int columns =
            std::max(
                1,
                (contentWidth + cardGap) /
                    (cardWidth + cardGap)
            );

        const int column =
            std::max(
                0,
                cardIndex
            ) %
            columns;

        const int row =
            std::max(
                0,
                cardIndex
            ) /
            columns;

        return SDL_Rect{
            overlayRect.x +
                28 +
                (
                    column *
                    (
                        cardWidth +
                        cardGap
                    )
                ),
            contentTop +
                54 +
                (
                    row *
                    (
                        cardHeight +
                        cardGap
                    )
                ),
            cardWidth,
            cardHeight
        };
    }

    SDL_Rect provinceArmyLevyRowRect(
        const SDL_Rect& overlayRect,
        int rowIndex,
        int scrollOffset
    )
    {
        const int contentTop =
            overlayRect.y + 92;

        const int rowHeight = 68;
        const int rowGap = 10;

        return SDL_Rect{
            overlayRect.x + 26,
            contentTop +
                12 +
                (
                    std::max(
                        0,
                        rowIndex
                    ) *
                    (
                        rowHeight +
                        rowGap
                    )
                ) -
                std::max(
                    0,
                    scrollOffset
                ),
            overlayRect.w - 52,
            rowHeight
        };
    }

    SDL_Rect provinceArmyLevyButtonRect(
        const SDL_Rect& overlayRect,
        int rowIndex,
        int scrollOffset
    )
    {
        const SDL_Rect rowRect =
            provinceArmyLevyRowRect(
                overlayRect,
                rowIndex,
                scrollOffset
            );

        return SDL_Rect{
            rowRect.x + rowRect.w - 118,
            rowRect.y + 16,
            102,
            36
        };
    }
}