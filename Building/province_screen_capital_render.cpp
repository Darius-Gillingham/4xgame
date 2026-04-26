// File: province_screen_capital_render.cpp
// Commit: Use centralized UiPalette building colors and remove hardcoded color logic from render layer

#include "province_screen_capital_render.h"
#include "province_screen_layout.h"
#include "ui.h"

#include <algorithm>
#include <string>
#include <vector>

namespace
{
    constexpr int kTreeIndent = 22;
    constexpr int kInnerGap = 6;
    constexpr int kInnerPad = 8;
    constexpr int kDividerHeight = 10;
    constexpr int kRootBlockPad = 10;

    bool hasReservedAttachedOnlyRow(BuildingType type)
    {
        return type == BuildingType::city || type == BuildingType::church;
    }

    std::string reservedAttachedOnlyLabel(BuildingType type)
    {
        if (type == BuildingType::city) return "Attached-only city building slot";
        if (type == BuildingType::church) return "Attached-only church building slot";
        return "Attached-only building slot";
    }

    int availableAttachmentSlots(const Province& province, int buildingId, BuildingType type)
    {
        if (!canHostAttachedBuildings(type)) return 0;

        const AttachmentCounts counts = province.attachmentCountsForParent(buildingId);
        const int capacity = directAttachmentSlotCapacity(type);
        return std::max(0, capacity - (counts.directSmallholders + counts.estates));
    }

    UiColor rootColor(BuildingType type)
    {
        switch (type)
        {
            case BuildingType::castle: return UiPalette::castleRoot;
            case BuildingType::noble_estate: return UiPalette::estateRoot;

            case BuildingType::tribe:
            case BuildingType::tribal_estate:
            case BuildingType::tribal_smallholder:
                return UiPalette::tribalRoot;

            case BuildingType::city: return UiPalette::cityRoot;
            case BuildingType::merchant_estate: return UiPalette::merchantEstateRoot;

            case BuildingType::church: return UiPalette::churchRoot;
            case BuildingType::religious_estate: return UiPalette::religiousEstateRoot;

            case BuildingType::smallholder: return UiPalette::smallholderRoot;

            default: return UiPalette::panelFill;
        }
    }

    UiColor rootBorder(BuildingType type)
    {
        if (type == BuildingType::church || type == BuildingType::religious_estate)
        {
            return UiPalette::border;
        }

        return UiPalette::activeBorder;
    }

    int contentHeightForBuildingTree(const Province& province, int buildingId, TTF_Font* bodyFont)
    {
        const Building* building = province.getBuildingById(buildingId);
        if (!building) return 0;

        const int bodyLine = std::max(18, lineHeightForFont(bodyFont, 18));
        const int headerHeight = bodyLine + 10;

        int height = 0;
        height += headerHeight + kInnerGap;

        if (canHostSubBuildings(building->type))
        {
            height += kSlotHeight + kInnerGap;
        }

        height += kDividerHeight + kInnerGap;

        for (int childId : province.directSmallholderIdsForParent(buildingId))
        {
            height += contentHeightForBuildingTree(province, childId, bodyFont) + kInnerGap;
        }

        for (int childId : province.estateIdsForParent(buildingId))
        {
            height += contentHeightForBuildingTree(province, childId, bodyFont) + kInnerGap;
        }

        const int emptySlots = availableAttachmentSlots(province, buildingId, building->type);

        if (isEstateBuildingType(building->type))
        {
            if (emptySlots > 0) height += kSlotHeight + kInnerGap;
        }
        else if (canHostAttachedBuildings(building->type))
        {
            if (emptySlots > 0) height += kSlotHeight + kInnerGap;

            if (hasReservedAttachedOnlyRow(building->type) && emptySlots > 1)
            {
                height += kSlotHeight + kInnerGap;
            }
        }

        return height;
    }

    void drawInheritedRow(
        SDL_Renderer* renderer,
        TTF_Font* bodyFont,
        const SDL_Rect& rect,
        BuildingType rootType,
        const std::string& label,
        bool clickable
    )
    {
        const UiColor fill = rootColor(rootType);
        const UiColor border = rootBorder(rootType);

        drawPanelFilled(renderer, rect, fill, border);

        if (clickable)
        {
            drawText(renderer, bodyFont, "+", rect.x + 10, rect.y + 5, UiPalette::text);
            drawText(renderer, bodyFont, label, rect.x + 30, rect.y + 5, UiPalette::text);
        }
        else
        {
            drawText(renderer, bodyFont, label, rect.x + 10, rect.y + 5, UiPalette::mutedText);
        }
    }
}

void drawAddRowButton(SDL_Renderer* renderer, TTF_Font* bodyFont, const SDL_Rect& rect, bool active)
{
    drawPanelFilled(renderer, rect, UiPalette::buttonFill, active ? UiPalette::activeBorder : UiPalette::border);

    drawText(renderer, bodyFont, "+", rect.x + 10, rect.y + 5, UiPalette::text);
    drawText(renderer, bodyFont, "Add Building", rect.x + 30, rect.y + 5, UiPalette::text);
}

void drawSlotRow(SDL_Renderer* renderer, TTF_Font* bodyFont, const SDL_Rect& rect, const std::string& label)
{
    drawPanelFilled(renderer, rect, UiPalette::slotFill, UiPalette::border);
    drawText(renderer, bodyFont, label, rect.x + 10, rect.y + 5, UiPalette::mutedText);
}

int renderBuildingTree(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Province& province,
    int buildingId,
    BuildingType rootType,
    int startX,
    int startY,
    int width,
    int depth,
    std::vector<ClickTarget>& clickTargets
)
{
    const Building* building = province.getBuildingById(buildingId);
    if (!building) return 0;

    const int indent = depth * kTreeIndent;
    const int x = startX + indent;
    const int w = std::max(160, width - indent);
    const int bodyLine = std::max(18, lineHeightForFont(bodyFont, 18));
    const int headerHeight = bodyLine + 10;

    const UiColor fill = rootColor(rootType);
    const UiColor border = rootBorder(rootType);

    int y = startY;

    SDL_Rect headerRect{x, y, w, headerHeight};
    drawPanelFilled(renderer, headerRect, fill, border);
    drawText(renderer, bodyFont, buildingDisplayName(*building), headerRect.x + kInnerPad, headerRect.y + 5, UiPalette::text);
    y += headerHeight + kInnerGap;

    if (canHostSubBuildings(building->type))
    {
        SDL_Rect subRow{x, y, w, kSlotHeight};
        drawPanelFilled(renderer, subRow, fill, border);
        drawText(renderer, bodyFont, "Sub-building slots", subRow.x + 10, subRow.y + 5, UiPalette::mutedText);
        y += kSlotHeight + kInnerGap;
    }

    setRenderColor(renderer, border);
    SDL_RenderDrawLine(renderer, x, y + (kDividerHeight / 2), x + w, y + (kDividerHeight / 2));
    y += kDividerHeight + kInnerGap;

    for (int childId : province.directSmallholderIdsForParent(buildingId))
    {
        y += renderBuildingTree(renderer, bodyFont, province, childId, rootType, startX, y, width, depth + 1, clickTargets);
        y += kInnerGap;
    }

    for (int childId : province.estateIdsForParent(buildingId))
    {
        y += renderBuildingTree(renderer, bodyFont, province, childId, rootType, startX, y, width, depth + 1, clickTargets);
        y += kInnerGap;
    }

    const int emptySlots = availableAttachmentSlots(province, buildingId, building->type);

    if (isEstateBuildingType(building->type))
    {
        if (emptySlots > 0)
        {
            SDL_Rect r{x, y, w, kSlotHeight};
            drawInheritedRow(renderer, bodyFont, r, rootType, "Add Building", true);
            clickTargets.push_back({r, BuildContextType::estate_attachment_slot, buildingId});
            y += kSlotHeight + kInnerGap;
        }
    }
    else if (canHostAttachedBuildings(building->type))
    {
        if (emptySlots > 0)
        {
            SDL_Rect r{x, y, w, kSlotHeight};
            drawInheritedRow(renderer, bodyFont, r, rootType, "Add Building", true);
            clickTargets.push_back({r, BuildContextType::direct_attachment_slot, buildingId});
            y += kSlotHeight + kInnerGap;
        }

        if (hasReservedAttachedOnlyRow(building->type) && emptySlots > 1)
        {
            SDL_Rect r{x, y, w, kSlotHeight};
            drawInheritedRow(renderer, bodyFont, r, rootType, reservedAttachedOnlyLabel(building->type), false);
            y += kSlotHeight + kInnerGap;
        }
    }

    return y - startY;
}

int renderBuildingRootBlock(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const Province& province,
    int buildingId,
    const SDL_Rect& sectionRect,
    int startY,
    std::vector<ClickTarget>& clickTargets
)
{
    const Building* root = province.getBuildingById(buildingId);
    if (!root) return 0;

    const int contentHeight = contentHeightForBuildingTree(province, buildingId, bodyFont);
    const int outerHeight = contentHeight + (kRootBlockPad * 2);

    SDL_Rect outer{
        sectionRect.x + kSectionPad,
        startY,
        sectionRect.w - (kSectionPad * 2),
        outerHeight
    };

    drawPanelFilled(renderer, outer, rootColor(root->type), rootBorder(root->type));

    renderBuildingTree(
        renderer,
        bodyFont,
        province,
        buildingId,
        root->type,
        outer.x + kRootBlockPad,
        outer.y + kRootBlockPad,
        outer.w - (kRootBlockPad * 2),
        0,
        clickTargets
    );

    return outerHeight;
}