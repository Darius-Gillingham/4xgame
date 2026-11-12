// File: window_input_click_province_build.cpp
// Commit: Use const levy traversal while preserving Army raise and lower interactions

#include "window_input_click_province.h"

#include "map_province_environment.h"
#include "window_helpers.h"

#include <string>
#include <vector>

bool processProvinceBuildingInteraction(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Map& map,
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

    if (
        windowState.provinceOverlay.activeTab ==
        ProvinceOverlayTab::army
    )
    {
        if (!ownedByPlayer)
        {
            windowState.shell.statusText =
                "Cannot raise or lower levies in an unowned province.";

            return true;
        }

        int rowIndex = 0;

        for (
            const Holding& holding :
            selectedProvince->holdings()
        )
        {
            for (
                const Building& building :
                holding.buildings
            )
            {
                for (
                    const Levy& levy :
                    building.levies
                )
                {
                    const SDL_Rect buttonRect =
                        WindowRender::provinceArmyLevyButtonRect(
                            overlayRect,
                            rowIndex,
                            windowState.provinceOverlay.scrollOffset
                        );

                    if (
                        WindowRender::pointInRect(
                            mouseX,
                            mouseY,
                            buttonRect
                        )
                    )
                    {
                        std::string reason;

                        if (levy.raised)
                        {
                            selectedProvince->lowerLevyFromBuilding(
                                holding.id,
                                building.id,
                                levy.slotIndex,
                                reason
                            );
                        }
                        else
                        {
                            selectedProvince->raiseLevyFromBuilding(
                                holding.id,
                                building.id,
                                levy.slotIndex,
                                reason
                            );
                        }

                        windowState.shell.statusText =
                            reason;

                        return true;
                    }

                    ++rowIndex;
                }
            }
        }

        return true;
    }

    if (windowState.selectingSubBuildingParentId >= 0)
    {
        if (!ownedByPlayer)
        {
            windowState.shell.statusText =
                "Cannot build in an unowned province.";

            windowState.selectingSubBuildingParentId = -1;

            return true;
        }

        const ProvinceEnvironmentSummary summary =
            summarizeProvinceEnvironment(
                map,
                windowState.selectedProvinceId
            );

        for (
            int i = 0;
            i < static_cast<int>(
                summary.availableBuildings.size()
            );
            ++i
        )
        {
            const SDL_Rect card =
                WindowRender::provinceSubBuildingCardRect(
                    overlayRect,
                    i
                );

            if (
                !WindowRender::pointInRect(
                    mouseX,
                    mouseY,
                    card
                )
            )
            {
                continue;
            }

            std::string reason;

            if (
                selectedProvince->buildBuilding(
                    windowState.selectingSubBuildingParentId,
                    summary.availableBuildings[
                        static_cast<std::size_t>(i)
                    ],
                    reason
                )
            )
            {
                windowState.provinceOverlay.scrollOffset = 0;
                windowState.selectingSubBuildingParentId = -1;
            }

            windowState.shell.statusText = reason;

            return true;
        }

        return true;
    }

    if (
        windowState.provinceOverlay.activeTab ==
            ProvinceOverlayTab::holdings &&
        !windowState.selectingRootBuilding &&
        WindowRender::pointInRect(
            mouseX,
            mouseY,
            dynamicAddBuildingButtonRect(
                overlayRect,
                *selectedProvince,
                bodyFont,
                windowState.provinceOverlay.scrollOffset
            )
        )
    )
    {
        if (!ownedByPlayer)
        {
            windowState.shell.statusText =
                "Cannot build in an unowned province.";
        }
        else
        {
            windowState.selectingRootBuilding = true;
            windowState.selectingSubBuildingParentId = -1;
            windowState.provinceOverlay.scrollOffset = 0;
            windowState.shell.statusText =
                "Select a holding to add.";
        }

        return true;
    }

    if (
        windowState.provinceOverlay.activeTab !=
            ProvinceOverlayTab::holdings ||
        selectedProvince->rootHoldingIds().empty()
    )
    {
        return true;
    }

    if (!ownedByPlayer)
    {
        windowState.shell.statusText =
            "Cannot build in an unowned province.";

        return true;
    }

    const std::vector<ClickTarget> clickTargets =
        collectProvinceBuildingClickTargets(
            renderer,
            bodyFont,
            overlayRect,
            *selectedProvince,
            windowState.provinceOverlay.scrollOffset
        );

    for (const ClickTarget& target : clickTargets)
    {
        if (
            !WindowRender::pointInRect(
                mouseX,
                mouseY,
                target.rect
            )
        )
        {
            continue;
        }

        if (
            target.contextType ==
            BuildContextType::building_slot
        )
        {
            windowState.selectingSubBuildingParentId =
                target.targetHoldingId;

            windowState.selectingRootBuilding = false;
            windowState.provinceOverlay.scrollOffset = 0;
            windowState.shell.statusText =
                "Select a building to add.";
        }

        break;
    }

    return true;
}