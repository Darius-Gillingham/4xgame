// File: window_render_overlay.cpp
// Commit: Pass province overlay scrolling into holdings, population, and army tab rendering

namespace WindowRender
{
    void drawProvinceOverlay(
        SDL_Renderer* renderer,
        TTF_Font* titleFont,
        TTF_Font* bodyFont,
        const SDL_Rect& overlayRect,
        const ProvinceOverlayState& overlay,
        const GameState& gameState,
        const Map& map,
        int selectedProvinceId,
        bool selectingRootBuilding,
        int selectingSubBuildingParentId)
    {
        drawPanel(renderer, overlayRect);

        drawProvinceOverlayHeaderTabs(
            renderer,
            titleFont,
            bodyFont,
            overlayRect,
            overlay
        );

        const int contentTop = overlayRect.y + 92;
        const int contentHeight = overlayRect.h - 106;

        const Province* selectedProvince =
            provinceById(
                gameState,
                selectedProvinceId
            );

        if (!selectedProvince)
        {
            drawProvinceMissingState(
                renderer,
                bodyFont,
                overlayRect,
                contentTop,
                contentHeight
            );
            return;
        }

        if (selectingRootBuilding)
        {
            drawRootBuildingSelection(
                renderer,
                titleFont,
                bodyFont,
                overlayRect,
                contentTop,
                contentHeight
            );
            return;
        }

        if (selectingSubBuildingParentId >= 0)
        {
            drawSubBuildingSelection(
                renderer,
                titleFont,
                bodyFont,
                overlayRect,
                map,
                selectedProvinceId,
                contentTop,
                contentHeight
            );

            return;
        }

        if (
            overlay.activeTab ==
            ProvinceOverlayTab::population
        )
        {
            drawProvincePopulationTabBody(
                renderer,
                bodyFont,
                overlayRect,
                map,
                selectedProvinceId,
                contentTop,
                contentHeight,
                overlay.scrollOffset,
                *selectedProvince
            );

            return;
        }

        if (
            overlay.activeTab ==
            ProvinceOverlayTab::army
        )
        {
            drawProvinceArmyTabBody(
                renderer,
                bodyFont,
                overlayRect,
                gameState,
                selectedProvinceId,
                contentTop,
                contentHeight,
                overlay.scrollOffset,
                *selectedProvince
            );

            return;
        }

        drawProvinceBuildingsTabBody(
            renderer,
            bodyFont,
            overlayRect,
            gameState,
            map,
            selectedProvinceId,
            contentTop,
            contentHeight,
            overlay.scrollOffset,
            *selectedProvince
        );
    }
}