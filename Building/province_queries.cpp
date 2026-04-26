// File: province_queries.cpp
// Commit: Support root queries and child queries for standalone and attached typed regular buildings

#include "province.h"

std::vector<int> Province::rootBuildingIds() const
{
    std::vector<int> ids;

    for (const Building& building : buildings_)
    {
        if (building.parentId < 0)
        {
            ids.push_back(building.id);
        }
    }

    return ids;
}

std::vector<int> Province::directSmallholderIdsForParent(int parentId) const
{
    std::vector<int> ids;

    const Building* parent = getBuildingById(parentId);
    if (!parent)
    {
        return ids;
    }

    for (int childId : parent->childIds)
    {
        const Building* child = getBuildingById(childId);
        if (child && isSmallholderBuildingType(child->type))
        {
            ids.push_back(childId);
        }
    }

    return ids;
}

std::vector<int> Province::estateIdsForParent(int parentId) const
{
    std::vector<int> ids;

    const Building* parent = getBuildingById(parentId);
    if (!parent)
    {
        return ids;
    }

    for (int childId : parent->childIds)
    {
        const Building* child = getBuildingById(childId);
        if (child && isEstateBuildingType(child->type))
        {
            ids.push_back(childId);
        }
    }

    return ids;
}

int Province::directSmallholderSlotCapacityForBuilding(int buildingId) const
{
    const Building* building = getBuildingById(buildingId);
    if (!building)
    {
        return 0;
    }

    return directAttachmentSlotCapacity(building->type);
}

int Province::estateSlotCapacityForBuilding(int buildingId) const
{
    const Building* building = getBuildingById(buildingId);
    if (!building)
    {
        return 0;
    }

    return directAttachmentSlotCapacity(building->type);
}