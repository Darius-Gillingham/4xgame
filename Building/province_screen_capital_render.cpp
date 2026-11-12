// File: building/province_screen_capital_render.cpp
// Commit: Expose non-rendering holding-tree content height calculations for scroll clamping

#include "province_screen_capital_render.h"
#include "province_screen_layout.h"
#include "ui.h"

#include <algorithm>
#include <string>
#include <vector>

namespace
{
    constexpr int kTreeIndent = 22;
    constexpr int kInnerGap = 8;
    constexpr int kInnerPad = 8;
    constexpr int kDividerHeight = 10;
    constexpr int kRootBlockPad = 10;
    constexpr int kBuildingCardHeight = 64;
    constexpr int kBuildingCardGap = 8;
    constexpr int kBuildingCardColumns = 2;

    int buildingTypeDisplayId(BuildingType type)
    {
        switch (type)
        {
            case BuildingType::explorer: return 1;
            case BuildingType::game_hunters: return 2;
            case BuildingType::fowl_hunters: return 3;
            case BuildingType::fishermen: return 4;
            case BuildingType::beachcombers: return 5;
            case BuildingType::gatherers: return 6;
            case BuildingType::loggers: return 7;
            case BuildingType::flint_knappers: return 8;
            case BuildingType::basalt_polishers: return 9;
            case BuildingType::game_butchers: return 10;
            case BuildingType::fowl_butchers: return 11;
            case BuildingType::feasting: return 12;
            case BuildingType::charcoal_maker: return 13;
            case BuildingType::potter: return 14;
            case BuildingType::tanner: return 15;
            case BuildingType::thatcher: return 16;
            case BuildingType::ropemaker: return 17;
            default: return 0;
        }
    }

    bool hasReservedAttachedOnlyRow(HoldingType type)
    {
        return
            type == HoldingType::city ||
            type == HoldingType::church;
    }

    std::string reservedAttachedOnlyLabel(HoldingType type)
    {
        if (type == HoldingType::city)
        {
            return "Attached-only city holding slot";
        }

        if (type == HoldingType::church)
        {
            return "Attached-only church holding slot";
        }

        return "Attached-only holding slot";
    }

    int availableAttachmentSlots(
        const Province& province,
        int holdingId,
        HoldingType type
    )
    {
        if (!canHostAttachedHoldings(type))
        {
            return 0;
        }

        const AttachmentCounts counts =
            province.attachmentCountsForParent(holdingId);

        const int capacity =
            directAttachmentSlotCapacity(type);

        return
            std::max(
                0,
                capacity -
                    (
                        counts.directSmallholders +
                        counts.estates
                    )
            );
    }

    UiColor rootColor(HoldingType type)
    {
        switch (type)
        {
            case HoldingType::castle:
                return UiPalette::castleRoot;

            case HoldingType::noble_estate:
                return UiPalette::estateRoot;

            case HoldingType::tribe:
            case HoldingType::tribal_estate:
            case HoldingType::tribal_smallholder:
                return UiPalette::tribalRoot;

            case HoldingType::sacred_site:
                return UiPalette::sacredSiteRoot;

            case HoldingType::city:
                return UiPalette::cityRoot;

            case HoldingType::merchant_estate:
                return UiPalette::merchantEstateRoot;

            case HoldingType::church:
                return UiPalette::churchRoot;

            case HoldingType::religious_estate:
                return UiPalette::religiousEstateRoot;

            case HoldingType::smallholder:
                return UiPalette::smallholderRoot;

            default:
                return UiPalette::panelFill;
        }
    }

    UiColor rootBorder(HoldingType type)
    {
        if (
            type == HoldingType::church ||
            type == HoldingType::religious_estate
        )
        {
            return UiPalette::border;
        }

        return UiPalette::activeBorder;
    }

    int buildingCardGridHeight(int slotCount)
    {
        if (slotCount <= 0)
        {
            return 0;
        }

        const int rows =
            (
                slotCount +
                kBuildingCardColumns -
                1
            ) /
            kBuildingCardColumns;

        return
            (rows * kBuildingCardHeight) +
            (
                (rows - 1) *
                kBuildingCardGap
            );
    }

    void drawInheritedRow(
        SDL_Renderer* renderer,
        TTF_Font* bodyFont,
        const SDL_Rect& rect,
        HoldingType rootType,
        const std::string& label,
        bool clickable
    )
    {
        const UiColor fill =
            rootColor(rootType);

        const UiColor border =
            rootBorder(rootType);

        drawPanelFilled(
            renderer,
            rect,
            fill,
            border
        );

        if (clickable)
        {
            drawText(
                renderer,
                bodyFont,
                "+",
                rect.x + 10,
                rect.y + 5,
                UiPalette::text
            );

            drawText(
                renderer,
                bodyFont,
                label,
                rect.x + 30,
                rect.y + 5,
                UiPalette::text
            );
        }
        else
        {
            drawText(
                renderer,
                bodyFont,
                label,
                rect.x + 10,
                rect.y + 5,
                UiPalette::mutedText
            );
        }
    }

    std::string buildingCardSummary(
        const Building& building
    )
    {
        const BuildingWorkProfile profile =
            workProfileForBuilding(
                building.type
            );

        int jobs = 0;

        for (
            const BuildingTier& tier :
            building.tiers
        )
        {
            jobs += tier.employedJobs;
        }

        return
            "ID " +
            std::to_string(
                buildingTypeDisplayId(
                    building.type
                )
            ) +
            " | Jobs " +
            std::to_string(jobs) +
            "/" +
            std::to_string(
                profile.jobCapacity
            );
    }

    void drawBuildingCard(
        SDL_Renderer* renderer,
        TTF_Font* bodyFont,
        const SDL_Rect& rect,
        const Building& building,
        UiColor border
    )
    {
        drawPanelFilled(
            renderer,
            rect,
            UiPalette::selectedFill,
            border
        );

        drawText(
            renderer,
            bodyFont,
            buildingDisplayName(building),
            rect.x + kInnerPad,
            rect.y + 7,
            UiPalette::text
        );

        drawText(
            renderer,
            bodyFont,
            buildingCardSummary(building),
            rect.x + kInnerPad,
            rect.y + 34,
            UiPalette::mutedText
        );
    }

    void drawEmptyBuildingCard(
        SDL_Renderer* renderer,
        TTF_Font* bodyFont,
        const SDL_Rect& rect,
        UiColor border
    )
    {
        drawPanelFilled(
            renderer,
            rect,
            UiPalette::slotFill,
            border
        );

        drawText(
            renderer,
            bodyFont,
            "+",
            rect.x + kInnerPad,
            rect.y + 7,
            UiPalette::text
        );

        drawText(
            renderer,
            bodyFont,
            "Build Building",
            rect.x + 28,
            rect.y + 7,
            UiPalette::text
        );

        drawText(
            renderer,
            bodyFont,
            "Empty production card",
            rect.x + kInnerPad,
            rect.y + 34,
            UiPalette::mutedText
        );
    }
}

int holdingTreeContentHeight(
    const Province& province,
    int holdingId,
    TTF_Font* bodyFont
)
{
    const Holding* holding =
        province.getHoldingById(holdingId);

    if (!holding)
    {
        return 0;
    }

    const int bodyLine =
        std::max(
            18,
            lineHeightForFont(
                bodyFont,
                18
            )
        );

    const int headerHeight =
        bodyLine + 10;

    int height = 0;

    height +=
        headerHeight +
        kInnerGap;

    if (canHostBuildings(holding->type))
    {
        height +=
            buildingCardGridHeight(
                buildingSlotCapacity(
                    holding->type
                )
            ) +
            kInnerGap;
    }

    height +=
        kDividerHeight +
        kInnerGap;

    for (
        int childId :
        province.directSmallholderIdsForParent(
            holdingId
        )
    )
    {
        height +=
            holdingTreeContentHeight(
                province,
                childId,
                bodyFont
            ) +
            kInnerGap;
    }

    for (
        int childId :
        province.estateIdsForParent(
            holdingId
        )
    )
    {
        height +=
            holdingTreeContentHeight(
                province,
                childId,
                bodyFont
            ) +
            kInnerGap;
    }

    const int emptySlots =
        availableAttachmentSlots(
            province,
            holdingId,
            holding->type
        );

    if (isEstateHoldingType(holding->type))
    {
        if (emptySlots > 0)
        {
            height +=
                kSlotHeight +
                kInnerGap;
        }
    }
    else if (canHostAttachedHoldings(holding->type))
    {
        if (emptySlots > 0)
        {
            height +=
                kSlotHeight +
                kInnerGap;
        }

        if (
            hasReservedAttachedOnlyRow(
                holding->type
            ) &&
            emptySlots > 1
        )
        {
            height +=
                kSlotHeight +
                kInnerGap;
        }
    }

    return height;
}

int holdingRootBlockHeight(
    const Province& province,
    int holdingId,
    TTF_Font* bodyFont
)
{
    if (!province.getHoldingById(holdingId))
    {
        return 0;
    }

    return
        holdingTreeContentHeight(
            province,
            holdingId,
            bodyFont
        ) +
        (kRootBlockPad * 2);
}

void drawAddRowButton(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SDL_Rect& rect,
    bool active
)
{
    drawPanelFilled(
        renderer,
        rect,
        UiPalette::buttonFill,
        active
            ? UiPalette::activeBorder
            : UiPalette::border
    );

    drawText(
        renderer,
        bodyFont,
        "+",
        rect.x + 10,
        rect.y + 5,
        UiPalette::text
    );

    drawText(
        renderer,
        bodyFont,
        "Add Holding",
        rect.x + 30,
        rect.y + 5,
        UiPalette::text
    );
}

void drawSlotRow(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SDL_Rect& rect,
    const std::string& label
)
{
    drawPanelFilled(
        renderer,
        rect,
        UiPalette::slotFill,
        UiPalette::border
    );

    drawText(
        renderer,
        bodyFont,
        label,
        rect.x + 10,
        rect.y + 5,
        UiPalette::mutedText
    );
}

int renderHoldingTree(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Province& province,
    int holdingId,
    HoldingType rootType,
    int startX,
    int startY,
    int width,
    int depth,
    std::vector<ClickTarget>& clickTargets
)
{
    const Holding* holding =
        province.getHoldingById(holdingId);

    if (!holding)
    {
        return 0;
    }

    const int indent =
        depth *
        kTreeIndent;

    const int x =
        startX +
        indent;

    const int w =
        std::max(
            160,
            width - indent
        );

    const int bodyLine =
        std::max(
            18,
            lineHeightForFont(
                bodyFont,
                18
            )
        );

    const int headerHeight =
        bodyLine + 10;

    const UiColor fill =
        rootColor(rootType);

    const UiColor border =
        rootBorder(rootType);

    int y = startY;

    SDL_Rect headerRect{
        x,
        y,
        w,
        headerHeight
    };

    drawPanelFilled(
        renderer,
        headerRect,
        fill,
        border
    );

    drawText(
        renderer,
        bodyFont,
        holdingDisplayName(*holding),
        headerRect.x + kInnerPad,
        headerRect.y + 5,
        UiPalette::text
    );

    y +=
        headerHeight +
        kInnerGap;

    if (canHostBuildings(holding->type))
    {
        const int slotCount =
            buildingSlotCapacity(
                holding->type
            );

        const int cardWidth =
            std::max(
                120,
                (
                    w -
                    (
                        (
                            kBuildingCardColumns -
                            1
                        ) *
                        kBuildingCardGap
                    )
                ) /
                kBuildingCardColumns
            );

        for (
            int slotIndex = 0;
            slotIndex < slotCount;
            ++slotIndex
        )
        {
            const int column =
                slotIndex %
                kBuildingCardColumns;

            const int row =
                slotIndex /
                kBuildingCardColumns;

            SDL_Rect cardRect{
                x +
                    (
                        column *
                        (
                            cardWidth +
                            kBuildingCardGap
                        )
                    ),
                y +
                    (
                        row *
                        (
                            kBuildingCardHeight +
                            kBuildingCardGap
                        )
                    ),
                cardWidth,
                kBuildingCardHeight
            };

            const bool occupied =
                slotIndex <
                static_cast<int>(
                    holding->buildings.size()
                );

            if (occupied)
            {
                const Building& building =
                    holding->buildings[
                        static_cast<size_t>(
                            slotIndex
                        )
                    ];

                drawBuildingCard(
                    renderer,
                    bodyFont,
                    cardRect,
                    building,
                    border
                );

                clickTargets.push_back(
                    {
                        cardRect,
                        BuildContextType::building_detail,
                        holdingId,
                        building.id
                    }
                );
            }
            else
            {
                drawEmptyBuildingCard(
                    renderer,
                    bodyFont,
                    cardRect,
                    border
                );

                clickTargets.push_back(
                    {
                        cardRect,
                        BuildContextType::building_slot,
                        holdingId,
                        -1
                    }
                );
            }
        }

        y +=
            buildingCardGridHeight(
                slotCount
            ) +
            kInnerGap;
    }

    setRenderColor(
        renderer,
        border
    );

    SDL_RenderDrawLine(
        renderer,
        x,
        y + (kDividerHeight / 2),
        x + w,
        y + (kDividerHeight / 2)
    );

    y +=
        kDividerHeight +
        kInnerGap;

    for (
        int childId :
        province.directSmallholderIdsForParent(
            holdingId
        )
    )
    {
        y +=
            renderHoldingTree(
                renderer,
                bodyFont,
                province,
                childId,
                rootType,
                startX,
                y,
                width,
                depth + 1,
                clickTargets
            );

        y += kInnerGap;
    }

    for (
        int childId :
        province.estateIdsForParent(
            holdingId
        )
    )
    {
        y +=
            renderHoldingTree(
                renderer,
                bodyFont,
                province,
                childId,
                rootType,
                startX,
                y,
                width,
                depth + 1,
                clickTargets
            );

        y += kInnerGap;
    }

    const int emptySlots =
        availableAttachmentSlots(
            province,
            holdingId,
            holding->type
        );

    if (isEstateHoldingType(holding->type))
    {
        if (emptySlots > 0)
        {
            SDL_Rect rowRect{
                x,
                y,
                w,
                kSlotHeight
            };

            drawInheritedRow(
                renderer,
                bodyFont,
                rowRect,
                rootType,
                "Add Holding",
                true
            );

            clickTargets.push_back(
                {
                    rowRect,
                    BuildContextType::estate_attachment_slot,
                    holdingId,
                    -1
                }
            );

            y +=
                kSlotHeight +
                kInnerGap;
        }
    }
    else if (canHostAttachedHoldings(holding->type))
    {
        if (emptySlots > 0)
        {
            SDL_Rect rowRect{
                x,
                y,
                w,
                kSlotHeight
            };

            drawInheritedRow(
                renderer,
                bodyFont,
                rowRect,
                rootType,
                "Add Holding",
                true
            );

            clickTargets.push_back(
                {
                    rowRect,
                    BuildContextType::direct_attachment_slot,
                    holdingId,
                    -1
                }
            );

            y +=
                kSlotHeight +
                kInnerGap;
        }

        if (
            hasReservedAttachedOnlyRow(
                holding->type
            ) &&
            emptySlots > 1
        )
        {
            SDL_Rect rowRect{
                x,
                y,
                w,
                kSlotHeight
            };

            drawInheritedRow(
                renderer,
                bodyFont,
                rowRect,
                rootType,
                reservedAttachedOnlyLabel(
                    holding->type
                ),
                false
            );

            y +=
                kSlotHeight +
                kInnerGap;
        }
    }

    return
        y -
        startY;
}

int renderHoldingRootBlock(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Province& province,
    int holdingId,
    const SDL_Rect& sectionRect,
    int startY,
    std::vector<ClickTarget>& clickTargets
)
{
    const Holding* root =
        province.getHoldingById(holdingId);

    if (!root)
    {
        return 0;
    }

    const int outerHeight =
        holdingRootBlockHeight(
            province,
            holdingId,
            bodyFont
        );

    SDL_Rect outer{
        sectionRect.x + kSectionPad,
        startY,
        sectionRect.w - (kSectionPad * 2),
        outerHeight
    };

    drawPanelFilled(
        renderer,
        outer,
        rootColor(root->type),
        rootBorder(root->type)
    );

    renderHoldingTree(
        renderer,
        bodyFont,
        province,
        holdingId,
        root->type,
        outer.x + kRootBlockPad,
        outer.y + kRootBlockPad,
        outer.w - (kRootBlockPad * 2),
        0,
        clickTargets
    );

    return outerHeight;
}