// File: building/province_build_subbuilding.cpp
// Commit: Add tribal sub-building construction and validation

#include "province.h"

bool Province::canBuildSubBuilding(
    int parentId,
    SubBuildingType type,
    std::string& reason) const
{
    const Building* parent = getBuildingById(parentId);

    if (!parent)
    {
        reason = "No valid parent selected.";
        return false;
    }

    if (!canHostSubBuildings(parent->type))
    {
        reason =
            std::string(buildingTypeName(parent->type)) +
            " cannot host sub-buildings.";
        return false;
    }

    if (!canBuildTribalSubBuilding(parent->type, type))
    {
        reason =
            std::string(subBuildingTypeName(type)) +
            " cannot be built in a " +
            buildingTypeName(parent->type) +
            ".";
        return false;
    }

    const int capacity = subBuildingSlotCapacity(parent->type);

    if (static_cast<int>(parent->subBuildings.size()) >= capacity)
    {
        reason =
            std::string(buildingDisplayName(*parent)) +
            " has no free sub-building slots.";
        return false;
    }

    reason =
        std::string("Build ") +
        subBuildingTypeName(type) +
        " in " +
        buildingDisplayName(*parent) +
        ".";

    return true;
}

bool Province::buildSubBuilding(
    int parentId,
    SubBuildingType type,
    std::string& reason)
{
    if (!canBuildSubBuilding(parentId, type, reason))
    {
        return false;
    }

    Building* parent = getBuildingById(parentId);

    if (!parent)
    {
        reason = "No valid parent selected.";
        return false;
    }

    SubBuilding subBuilding;
    subBuilding.id = nextSubBuildingId_++;
    subBuilding.type = type;

    parent->subBuildings.push_back(subBuilding);

    reason =
        std::string("Built ") +
        subBuildingDisplayName(subBuilding) +
        ".";

    return true;
}