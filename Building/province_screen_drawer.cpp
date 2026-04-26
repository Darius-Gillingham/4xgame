// File: province_screen_drawer.cpp
// Commit: Make both root build entry points use the full standalone building list and keep attachment-only filtering on attachment widgets

#include "province_screen_drawer.h"

std::vector<BuildOption> buildOptionsForContext(const Province& province, const BuildDrawerState& drawer)
{
    std::vector<BuildOption> options;

    if (
        drawer.contextType == BuildContextType::root_capital ||
        drawer.contextType == BuildContextType::normal_building_section
    )
    {
        for (BuildingType type : allowedRootBuildingTypes())
        {
            std::string reason;
            const bool enabled = province.canBuildRoot(type, reason);

            BuildOption option;
            option.label = buildingTypeName(type);
            option.description = reason;
            option.enabled = enabled;
            option.buildingType = type;
            options.push_back(option);
        }
    }
    else if (drawer.contextType == BuildContextType::direct_attachment_slot)
    {
        const Building* parent = province.getBuildingById(drawer.targetBuildingId);
        if (!parent)
        {
            return options;
        }

        std::string smallholderReason;
        const bool smallholderEnabled = province.canBuildSmallholderAttachment(drawer.targetBuildingId, smallholderReason);
        for (BuildingType type : allowedSmallholderTypesForParent(parent->type))
        {
            BuildOption option;
            option.label = buildingTypeName(type);
            option.description = smallholderReason;
            option.enabled = smallholderEnabled;
            option.buildingType = type;
            options.push_back(option);
        }

        std::string estateReason;
        const bool estateEnabled = province.canBuildEstateAttachment(drawer.targetBuildingId, estateReason);
        for (BuildingType type : allowedEstateTypesForParent(parent->type))
        {
            BuildOption option;
            option.label = buildingTypeName(type);
            option.description = estateReason;
            option.enabled = estateEnabled;
            option.buildingType = type;
            options.push_back(option);
        }
    }
    else if (drawer.contextType == BuildContextType::estate_attachment_slot)
    {
        const Building* parent = province.getBuildingById(drawer.targetBuildingId);
        if (!parent)
        {
            return options;
        }

        std::string reason;
        const bool enabled = province.canBuildSmallholderAttachment(drawer.targetBuildingId, reason);
        for (BuildingType type : allowedSmallholderTypesForParent(parent->type))
        {
            BuildOption option;
            option.label = buildingTypeName(type);
            option.description = reason;
            option.enabled = enabled;
            option.buildingType = type;
            options.push_back(option);
        }
    }

    return options;
}

void openRootDrawer(BuildDrawerState& drawer)
{
    drawer.contextType = BuildContextType::root_capital;
    drawer.targetBuildingId = -1;
    drawer.scrollOffset = 0;
    drawer.title = "Build Building";
    drawer.subtitle = "Choose any standalone building for the province.";
}

void openDirectAttachmentDrawer(BuildDrawerState& drawer, const Province& province, int targetBuildingId)
{
    const Building* building = province.getBuildingById(targetBuildingId);

    drawer.contextType = BuildContextType::direct_attachment_slot;
    drawer.targetBuildingId = targetBuildingId;
    drawer.scrollOffset = 0;
    drawer.title = "Attach Building";
    drawer.subtitle = std::string("Target: ") + (building ? buildingDisplayName(*building) : "Unknown");
}

void openEstateAttachmentDrawer(BuildDrawerState& drawer, const Province& province, int targetBuildingId)
{
    const Building* building = province.getBuildingById(targetBuildingId);

    drawer.contextType = BuildContextType::estate_attachment_slot;
    drawer.targetBuildingId = targetBuildingId;
    drawer.scrollOffset = 0;
    drawer.title = "Attach Smallholder";
    drawer.subtitle = std::string("Target: ") + (building ? buildingDisplayName(*building) : "Unknown");
}

void openNormalBuildingsDrawer(BuildDrawerState& drawer)
{
    drawer.contextType = BuildContextType::normal_building_section;
    drawer.targetBuildingId = -1;
    drawer.scrollOffset = 0;
    drawer.title = "Build Building";
    drawer.subtitle = "Choose any standalone building for the province.";
}

bool applyDrawerBuildSelection(
    Province& province,
    const BuildDrawerState& drawer,
    const BuildOption& option,
    std::string& resultText
)
{
    if (!option.enabled)
    {
        resultText = option.description;
        return false;
    }

    bool built = false;
    std::string reason;

    if (
        drawer.contextType == BuildContextType::root_capital ||
        drawer.contextType == BuildContextType::normal_building_section
    )
    {
        built = province.buildRoot(option.buildingType, reason);
    }
    else if (drawer.contextType == BuildContextType::direct_attachment_slot)
    {
        if (isSmallholderBuildingType(option.buildingType))
        {
            built = province.buildSmallholderAttachment(drawer.targetBuildingId, reason);
        }
        else if (isEstateBuildingType(option.buildingType))
        {
            built = province.buildEstateAttachment(drawer.targetBuildingId, reason);
        }
    }
    else if (drawer.contextType == BuildContextType::estate_attachment_slot)
    {
        if (isSmallholderBuildingType(option.buildingType))
        {
            built = province.buildSmallholderAttachment(drawer.targetBuildingId, reason);
        }
    }

    resultText = reason;
    return built;
}