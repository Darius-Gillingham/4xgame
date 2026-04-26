// File: province_build_root.cpp
// Commit: Remove root type uniqueness restriction so provinces can have unlimited root buildings of every type

#include "province.h"

bool Province::hasCapitalType(BuildingType type) const
{
    (void)type;
    return false;
}

bool Province::canBuildRoot(BuildingType type, std::string& reason) const
{
    if (!canBuildAsRoot(type))
    {
        reason = std::string(buildingTypeName(type)) + " cannot be built as a root building.";
        return false;
    }

    reason = std::string("Build ") + buildingTypeName(type) + " as a standalone root building.";
    return true;
}

bool Province::buildRoot(BuildingType type, std::string& reason)
{
    if (!canBuildRoot(type, reason))
    {
        return false;
    }

    Building building;
    building.id = nextBuildingId_++;
    building.type = type;
    building.parentId = -1;
    buildings_.push_back(building);

    const Building* built = getBuildingById(building.id);
    reason = std::string("Built ") + (built ? buildingDisplayName(*built) : std::string(buildingTypeName(type))) + ".";
    return true;
}