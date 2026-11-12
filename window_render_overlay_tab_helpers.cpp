// File: window_render_overlay_tab_helpers.cpp
// Commit: Remove defunct Holdings side summary panel and use full content width

namespace WindowRender
{
    namespace
    {
        int fontLineHeight(
            TTF_Font* font,
            int fallbackHeight
        )
        {
            if (!font)
            {
                return fallbackHeight;
            }

            return
                std::max(
                    fallbackHeight,
                    TTF_FontLineSkip(font)
                );
        }

        int multilineTextHeight(
            TTF_Font* bodyFont,
            const std::string& text
        )
        {
            int lineCount = 1;

            for (char character : text)
            {
                if (character == '\n')
                {
                    ++lineCount;
                }
            }

            return
                lineCount *
                fontLineHeight(
                    bodyFont,
                    18
                );
        }

        std::string provincePopulationSummary(
            const Map& map,
            int selectedProvinceId,
            const Province& province
        )
        {
            return
                "Population: " +
                std::to_string(
                    province.totalPopulation()
                ) +
                "\nUnassigned: " +
                std::to_string(
                    province.unassignedPopulation()
                ) +
                "\nEmployed: " +
                std::to_string(
                    province.employedPopulation()
                ) +
                "\nAvailable Workers: " +
                std::to_string(
                    province.availableWorkerPopulation()
                ) +
                "\n\n" +
                environmentSummaryText(
                    map,
                    selectedProvinceId
                );
        }

        void drawProvinceBuildingsTabBody(
            SDL_Renderer* renderer,
            TTF_Font* bodyFont,
            const SDL_Rect& overlayRect,
            const GameState& gameState,
            const Map& map,
            int selectedProvinceId,
            int contentTop,
            int contentHeight,
            int scrollOffset,
            const Province& province
        )
        {
            (void)map;

            const bool ownedByPlayer =
                isSelectedProvinceOwnedByPlayer(
                    gameState,
                    selectedProvinceId
                );

            SDL_Rect holdingPanel{
                overlayRect.x + 14,
                contentTop,
                overlayRect.w - 28,
                contentHeight
            };

            drawPanel(
                renderer,
                holdingPanel
            );

            drawText(
                renderer,
                bodyFont,
                "Holdings",
                holdingPanel.x + 12,
                holdingPanel.y + 10,
                SDL_Color{0, 0, 0, 255}
            );

            std::vector<ClickTarget>
                ignoredClickTargets;

            int y =
                holdingPanel.y +
                44 -
                scrollOffset;

            for (
                int rootId :
                province.rootHoldingIds()
            )
            {
                y +=
                    ::renderHoldingRootBlock(
                        renderer,
                        bodyFont,
                        province,
                        rootId,
                        holdingPanel,
                        y,
                        ignoredClickTargets
                    );

                y += 14;
            }

            const SDL_Rect addHoldingButton =
                provinceAddBuildingButtonRectAtY(
                    overlayRect,
                    y
                );

            if (
                addHoldingButton.y +
                    addHoldingButton.h >=
                holdingPanel.y &&
                addHoldingButton.y <=
                holdingPanel.y +
                    holdingPanel.h
            )
            {
                drawButton(
                    renderer,
                    bodyFont,
                    addHoldingButton,
                    ownedByPlayer
                        ? "Add Holding"
                        : "Unowned Province",
                    ownedByPlayer
                );
            }
        }

        void drawProvincePopulationTabBody(
            SDL_Renderer* renderer,
            TTF_Font* bodyFont,
            const SDL_Rect& overlayRect,
            const Map& map,
            int selectedProvinceId,
            int contentTop,
            int contentHeight,
            int scrollOffset,
            const Province& province
        )
        {
            SDL_Rect populationPanel{
                overlayRect.x + 14,
                contentTop,
                overlayRect.w - 28,
                contentHeight
            };

            drawPanel(
                renderer,
                populationPanel
            );

            const std::string summary =
                provincePopulationSummary(
                    map,
                    selectedProvinceId,
                    province
                );

            drawText(
                renderer,
                bodyFont,
                summary,
                populationPanel.x + 12,
                populationPanel.y +
                    12 -
                    scrollOffset,
                SDL_Color{0, 0, 0, 255}
            );
        }

        void drawProvinceArmyTabBody(
            SDL_Renderer* renderer,
            TTF_Font* bodyFont,
            const SDL_Rect& overlayRect,
            const GameState& gameState,
            int selectedProvinceId,
            int contentTop,
            int contentHeight,
            int scrollOffset,
            const Province& province
        )
        {
            SDL_Rect armyPanel{
                overlayRect.x + 14,
                contentTop,
                overlayRect.w - 28,
                contentHeight
            };

            drawPanel(
                renderer,
                armyPanel
            );

            const bool ownedByPlayer =
                isSelectedProvinceOwnedByPlayer(
                    gameState,
                    selectedProvinceId
                );

            int totalCount = 0;
            int raisedCount = 0;

            for (const Holding& holding : province.holdings())
            {
                for (const Building& building : holding.buildings)
                {
                    totalCount +=
                        static_cast<int>(
                            building.levies.size()
                        );

                    raisedCount +=
                        province.raisedLevyCountForBuilding(
                            holding.id,
                            building.id
                        );
                }
            }

            const std::string summary =
                "Army Levies: " +
                std::to_string(raisedCount) +
                " raised / " +
                std::to_string(totalCount) +
                " total";

            drawText(
                renderer,
                bodyFont,
                summary,
                armyPanel.x + 12,
                armyPanel.y + 10,
                SDL_Color{0, 0, 0, 255}
            );

            if (totalCount <= 0)
            {
                drawText(
                    renderer,
                    bodyFont,
                    "No building levies are available in this province.",
                    armyPanel.x + 12,
                    armyPanel.y + 44,
                    SDL_Color{0, 0, 0, 255}
                );

                return;
            }

            int rowIndex = 0;

            for (const Holding& holding : province.holdings())
            {
                for (const Building& building : holding.buildings)
                {
                    for (const Levy& levy : building.levies)
                    {
                        const SDL_Rect rowRect =
                            provinceArmyLevyRowRect(
                                overlayRect,
                                rowIndex,
                                scrollOffset
                            );

                        const SDL_Rect buttonRect =
                            provinceArmyLevyButtonRect(
                                overlayRect,
                                rowIndex,
                                scrollOffset
                            );

                        if (
                            rowRect.y +
                                rowRect.h >=
                            armyPanel.y + 38 &&
                            rowRect.y <=
                            armyPanel.y +
                                armyPanel.h
                        )
                        {
                            drawPanel(
                                renderer,
                                rowRect
                            );

                            const std::string levyName =
                                "Levy " +
                                std::to_string(
                                    levy.slotIndex + 1
                                ) +
                                " - " +
                                buildingDisplayName(
                                    building
                                );

                            drawText(
                                renderer,
                                bodyFont,
                                levyName,
                                rowRect.x + 12,
                                rowRect.y + 10,
                                SDL_Color{0, 0, 0, 255}
                            );

                            const std::string levySource =
                                holdingDisplayName(
                                    holding
                                ) +
                                " | Status: " +
                                (
                                    levy.raised
                                        ? "Raised"
                                        : "Available"
                                );

                            drawText(
                                renderer,
                                bodyFont,
                                levySource,
                                rowRect.x + 12,
                                rowRect.y + 36,
                                SDL_Color{0, 0, 0, 255}
                            );

                            drawButton(
                                renderer,
                                bodyFont,
                                buttonRect,
                                levy.raised
                                    ? "Lower"
                                    : "Raise",
                                ownedByPlayer
                            );
                        }

                        ++rowIndex;
                    }
                }
            }
        }
    }

    int provinceHoldingsContentHeight(
        TTF_Font* bodyFont,
        const SDL_Rect& overlayRect,
        const Province& province
    )
    {
        int contentHeight = 44;

        for (
            int rootId :
            province.rootHoldingIds()
        )
        {
            contentHeight +=
                ::holdingRootBlockHeight(
                    province,
                    rootId,
                    bodyFont
                );

            contentHeight += 14;
        }

        contentHeight +=
            provinceAddBuildingButtonRectAtY(
                overlayRect,
                0
            ).h;

        contentHeight += 12;

        return contentHeight;
    }

    int provincePopulationContentHeight(
        TTF_Font* bodyFont,
        const Map& map,
        int selectedProvinceId,
        const Province& province
    )
    {
        const std::string summary =
            provincePopulationSummary(
                map,
                selectedProvinceId,
                province
            );

        return
            24 +
            multilineTextHeight(
                bodyFont,
                summary
            );
    }

    int provinceArmyContentHeight(
        TTF_Font* bodyFont,
        const Province& province
    )
    {
        const int rowHeight = 68;
        const int rowGap = 10;

        const int summaryHeight =
            std::max(
                38,
                fontLineHeight(
                    bodyFont,
                    18
                ) +
                    20
            );

        int levyCount = 0;

        for (const Holding& holding : province.holdings())
        {
            for (const Building& building : holding.buildings)
            {
                levyCount +=
                    static_cast<int>(
                        building.levies.size()
                    );
            }
        }

        if (levyCount <= 0)
        {
            return
                summaryHeight +
                fontLineHeight(
                    bodyFont,
                    18
                ) +
                24;
        }

        return
            summaryHeight +
            12 +
            (
                levyCount *
                (
                    rowHeight +
                    rowGap
                )
            ) +
            12;
    }
}