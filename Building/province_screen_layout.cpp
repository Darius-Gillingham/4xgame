// File: province_screen_layout.cpp
// Commit: Include outer root block padding in building section height so colored root wrappers measure correctly

#include "province_screen_layout.h"
#include "building.h"

#include <algorithm>
#include <string>
#include <vector>

const int kOuterPadding = 16;
const int kColumnGap = 16;
const int kRowGap = 16;
const int kHeaderHeight = 72;
const int kDrawerWidth = 360;
const int kSectionPad = 14;
const int kSlotHeight = 30;

namespace
{
    constexpr int kTreeInnerGap = 6;
    constexpr int kDividerHeight = 10;
    constexpr int kRootBlockPad = 10;
}

int lineHeightForFont(TTF_Font* font, int fallback)
{
    if (!font)
    {
        return fallback;
    }

    const int value = TTF_FontHeight(font);
    return value > 0 ? value : fallback;
}

int measureWrappedHeight(TTF_Font* font, const std::string& text, int wrapWidth, int fallback)
{
    if (!font)
    {
        return fallback;
    }

    int w = 0;
    int h = 0;
    if (TTF_SizeUTF8(font, text.c_str(), &w, &h) == 0 && w <= wrapWidth)
    {
        return h;
    }

    const int lineHeight = std::max(18, lineHeightForFont(font, fallback));
    int estimatedLines = 1;

    if (wrapWidth > 0)
    {
        estimatedLines = std::max(1, static_cast<int>(text.size()) / std::max(10, wrapWidth / 10));
    }

    return lineHeight * estimatedLines;
}

static std::string resourcesSummaryText()
{
    return
        "No resources added yet.\n"
        "Outputs: 0\n"
        "Inputs: 0\n"
        "Net: 0";
}

static std::string metricsSummaryText()
{
    return
        "Convoy Power: 0\n"
        "Caravan Power: 0\n"
        "Consumption Capacity: 0\n"
        "Lending Capacity: 0";
}

static bool hasReservedAttachedOnlyRow(BuildingType type)
{
    return type == BuildingType::city || type == BuildingType::church;
}

static int renderedAttachmentRowCount(const Province& province, int buildingId)
{
    const Building* building = province.getBuildingById(buildingId);
    if (!building || !canHostAttachedBuildings(building->type))
    {
        return 0;
    }

    const AttachmentCounts counts = province.attachmentCountsForParent(buildingId);
    const int capacity = directAttachmentSlotCapacity(building->type);
    const int emptySlots = std::max(0, capacity - (counts.directSmallholders + counts.estates));

    if (emptySlots <= 0)
    {
        return 0;
    }

    if (isEstateBuildingType(building->type))
    {
        return 1;
    }

    if (hasReservedAttachedOnlyRow(building->type))
    {
        return emptySlots > 1 ? 2 : 1;
    }

    return 1;
}

static int buildingTreeContentHeight(const Province& province, int buildingId, TTF_Font* bodyFont)
{
    const Building* building = province.getBuildingById(buildingId);
    if (!building)
    {
        return 0;
    }

    const int bodyLine = std::max(18, lineHeightForFont(bodyFont, 18));
    const int headerHeight = bodyLine + 10;

    int height = 0;
    height += headerHeight + kTreeInnerGap;

    if (canHostSubBuildings(building->type))
    {
        height += kSlotHeight + kTreeInnerGap;
    }

    height += kDividerHeight + kTreeInnerGap;

    for (int childId : province.directSmallholderIdsForParent(buildingId))
    {
        height += buildingTreeContentHeight(province, childId, bodyFont) + kTreeInnerGap;
    }

    for (int childId : province.estateIdsForParent(buildingId))
    {
        height += buildingTreeContentHeight(province, childId, bodyFont) + kTreeInnerGap;
    }

    height += renderedAttachmentRowCount(province, buildingId) * (kSlotHeight + kTreeInnerGap);

    return height;
}

static int buildingRootBlockHeight(const Province& province, int buildingId, TTF_Font* bodyFont)
{
    return buildingTreeContentHeight(province, buildingId, bodyFont) + (kRootBlockPad * 2);
}

int capitalSectionContentHeight(const Province& province, TTF_Font* bodyFont)
{
    int height = 0;

    for (int rootId : province.rootBuildingIds())
    {
        height += buildingRootBlockHeight(province, rootId, bodyFont) + 14;
    }

    return height;
}

int normalBuildingsSectionContentHeight(TTF_Font* bodyFont)
{
    return measureWrappedHeight(
        bodyFont,
        "This menu now opens the full standalone building list. "
        "Only attachment widgets should restrict the menu to attachment-valid buildings. "
        "City- and church-only attached building types are not added yet.",
        400,
        22
    ) + 18 + kSlotHeight;
}

int infrastructureSectionContentHeight(TTF_Font* bodyFont)
{
    return measureWrappedHeight(
        bodyFont,
        "No infrastructure building types added yet.",
        400,
        22
    ) + 16;
}

int popCountSectionContentHeight(TTF_Font* bodyFont)
{
    return measureWrappedHeight(
        bodyFont,
        "Peasants: 200\nFree Peasants: 200",
        280,
        22
    ) + 16;
}

int resourcesSectionContentHeight(TTF_Font* bodyFont)
{
    return measureWrappedHeight(bodyFont, resourcesSummaryText(), 280, 22) + 16;
}

int metricsSectionContentHeight(TTF_Font* bodyFont)
{
    return measureWrappedHeight(bodyFont, metricsSummaryText(), 280, 22) + 16;
}