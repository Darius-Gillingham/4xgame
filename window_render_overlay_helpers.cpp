// File: window_render_overlay_helpers.cpp
// Commit: Add the Army tab to the live province overlay header

namespace WindowRender
{
    namespace
    {
        void drawProvinceOverlayHeaderTabs(
            SDL_Renderer* renderer,
            TTF_Font* titleFont,
            TTF_Font* bodyFont,
            const SDL_Rect& overlayRect,
            const ProvinceOverlayState& overlay
        )
        {
            drawText(
                renderer,
                titleFont,
                "Province",
                overlayRect.x + 14,
                overlayRect.y + 10,
                SDL_Color{0, 0, 0, 255}
            );

            SDL_Rect closeButton{
                overlayRect.x + overlayRect.w - 42,
                overlayRect.y + 10,
                28,
                28
            };

            drawButton(
                renderer,
                bodyFont,
                closeButton,
                "X",
                false
            );

            drawButton(
                renderer,
                bodyFont,
                provinceBuildingsTabRect(
                    overlayRect
                ),
                "Holdings",
                overlay.activeTab ==
                    ProvinceOverlayTab::holdings
            );

            drawButton(
                renderer,
                bodyFont,
                provincePopulationTabRect(
                    overlayRect
                ),
                "Population",
                overlay.activeTab ==
                    ProvinceOverlayTab::population
            );

            drawButton(
                renderer,
                bodyFont,
                provinceArmyTabRect(
                    overlayRect
                ),
                "Army",
                overlay.activeTab ==
                    ProvinceOverlayTab::army
            );
        }

        void drawProvinceMissingState(
            SDL_Renderer* renderer,
            TTF_Font* bodyFont,
            const SDL_Rect& overlayRect,
            int contentTop,
            int contentHeight
        )
        {
            SDL_Rect missingPanel{
                overlayRect.x + 14,
                contentTop,
                overlayRect.w - 28,
                contentHeight
            };

            drawPanel(
                renderer,
                missingPanel
            );

            drawText(
                renderer,
                bodyFont,
                "Province state not found.",
                missingPanel.x + 12,
                missingPanel.y + 12,
                SDL_Color{0, 0, 0, 255}
            );
        }

        void drawRootBuildingSelection(
            SDL_Renderer* renderer,
            TTF_Font* titleFont,
            TTF_Font* bodyFont,
            const SDL_Rect& overlayRect,
            int contentTop,
            int contentHeight
        )
        {
            SDL_Rect selectionPanel{
                overlayRect.x + 14,
                contentTop,
                overlayRect.w - 28,
                contentHeight
            };

            drawPanel(
                renderer,
                selectionPanel
            );

            drawText(
                renderer,
                titleFont,
                "Select Holding",
                selectionPanel.x + 12,
                selectionPanel.y + 12,
                SDL_Color{0, 0, 0, 255}
            );

            const SDL_Rect sacredSiteCard =
                provinceSacredSiteCardRect(
                    overlayRect
                );

            drawPanel(
                renderer,
                sacredSiteCard
            );

            drawText(
                renderer,
                titleFont,
                "Sacred Site",
                sacredSiteCard.x + 14,
                sacredSiteCard.y + 12,
                SDL_Color{0, 0, 0, 255}
            );

            drawText(
                renderer,
                bodyFont,
                "Creates a sacred root holding.\nCost: Prestige",
                sacredSiteCard.x + 14,
                sacredSiteCard.y + 48,
                SDL_Color{0, 0, 0, 255}
            );

            SDL_Rect chooseButton{
                sacredSiteCard.x + 14,
                sacredSiteCard.y +
                    sacredSiteCard.h -
                    44,
                sacredSiteCard.w - 28,
                32
            };

            drawButton(
                renderer,
                bodyFont,
                chooseButton,
                "Build",
                true
            );
        }

        void drawSubBuildingSelection(
            SDL_Renderer* renderer,
            TTF_Font* titleFont,
            TTF_Font* bodyFont,
            const SDL_Rect& overlayRect,
            const Map& map,
            int selectedProvinceId,
            int contentTop,
            int contentHeight
        )
        {
            SDL_Rect selectionPanel{
                overlayRect.x + 14,
                contentTop,
                overlayRect.w - 28,
                contentHeight
            };

            drawPanel(
                renderer,
                selectionPanel
            );

            drawText(
                renderer,
                titleFont,
                "Select Building",
                selectionPanel.x + 12,
                selectionPanel.y + 12,
                SDL_Color{0, 0, 0, 255}
            );

            const ProvinceEnvironmentSummary summary =
                summarizeProvinceEnvironment(
                    map,
                    selectedProvinceId
                );

            if (summary.availableBuildings.empty())
            {
                drawText(
                    renderer,
                    bodyFont,
                    "No buildings are available for this province.",
                    selectionPanel.x + 12,
                    selectionPanel.y + 56,
                    SDL_Color{0, 0, 0, 255}
                );

                return;
            }

            for (
                int i = 0;
                i <
                    static_cast<int>(
                        summary.availableBuildings.size()
                    );
                ++i
            )
            {
                const BuildingType buildingType =
                    summary.availableBuildings[
                        static_cast<std::size_t>(
                            i
                        )
                    ];

                const SDL_Rect buildingCard =
                    provinceSubBuildingCardRect(
                        overlayRect,
                        i
                    );

                if (
                    buildingCard.y +
                    buildingCard.h >
                    selectionPanel.y +
                    selectionPanel.h -
                    12
                )
                {
                    break;
                }

                drawPanel(
                    renderer,
                    buildingCard
                );

                drawText(
                    renderer,
                    titleFont,
                    buildingTypeName(
                        buildingType
                    ),
                    buildingCard.x + 14,
                    buildingCard.y + 12,
                    SDL_Color{0, 0, 0, 255}
                );

                drawText(
                    renderer,
                    bodyFont,
                    "Adds this building inside the selected holding.\nCost: Prestige",
                    buildingCard.x + 14,
                    buildingCard.y + 48,
                    SDL_Color{0, 0, 0, 255}
                );

                SDL_Rect chooseButton{
                    buildingCard.x + 14,
                    buildingCard.y +
                        buildingCard.h -
                        44,
                    buildingCard.w - 28,
                    32
                };

                drawButton(
                    renderer,
                    bodyFont,
                    chooseButton,
                    "Build",
                    true
                );
            }
        }
    }
}