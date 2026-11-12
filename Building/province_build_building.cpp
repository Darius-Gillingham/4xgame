// File: building/province_build_building.cpp
// Commit: Rename former sub-building construction file to current building terminology

#include "province.h"

#include <algorithm>

namespace
{
    constexpr int kBuildingPrestigeCost = 10;
}

bool Province::canBuildBuilding(
    int parentId,
    BuildingType type,
    std::string& reason
) const
{
    const Holding* parent =
        getHoldingById(parentId);

    if (!parent)
    {
        reason = "No valid parent selected.";
        return false;
    }

    if (!canHostBuildings(parent->type))
    {
        reason =
            std::string(holdingTypeName(parent->type)) +
            " cannot host buildings.";

        return false;
    }

    if (!canBuildTribalBuilding(parent->type, type))
    {
        reason =
            std::string(buildingTypeName(type)) +
            " cannot be built in a " +
            holdingTypeName(parent->type) +
            ".";

        return false;
    }

    const int capacity =
        buildingSlotCapacity(parent->type);

    if (static_cast<int>(parent->buildings.size()) >= capacity)
    {
        reason =
            holdingDisplayName(*parent) +
            " has no free building slots.";

        return false;
    }

    if (prestige() < kBuildingPrestigeCost)
    {
        reason =
            std::string("Not enough Prestige. Requires ") +
            std::to_string(kBuildingPrestigeCost) +
            " Prestige.";

        return false;
    }

    reason =
        std::string("Build ") +
        buildingTypeName(type) +
        " in " +
        holdingDisplayName(*parent) +
        ". Cost: " +
        std::to_string(kBuildingPrestigeCost) +
        " Prestige.";

    return true;
}

bool Province::buildBuilding(
    int parentId,
    BuildingType type,
    std::string& reason
)
{
    if (!canBuildBuilding(parentId, type, reason))
    {
        return false;
    }

    Holding* parent =
        getHoldingById(parentId);

    if (!parent)
    {
        reason = "No valid parent selected.";
        return false;
    }

    if (!spendPrestige(kBuildingPrestigeCost))
    {
        reason =
            std::string("Not enough Prestige. Requires ") +
            std::to_string(kBuildingPrestigeCost) +
            " Prestige.";

        return false;
    }

    const BuildingWorkProfile profile =
        workProfileForBuilding(type);

    Building building;
    building.id = nextBuildingId_++;
    building.type = type;

    BuildingTier tier;
    tier.level = 1;
    tier.count = 1;
    tier.employedJobs = 0;
    building.tiers.push_back(tier);

    const int buildingId = building.id;
    parent->buildings.push_back(building);

    std::string assignReason;

    assignWorkersToBuilding(
        parentId,
        buildingId,
        profile.jobCapacity,
        assignReason
    );

    const Building* builtBuilding = nullptr;

    const Holding* refreshedParent =
        getHoldingById(parentId);

    if (refreshedParent)
    {
        for (const Building& existingBuilding : refreshedParent->buildings)
        {
            if (existingBuilding.id == buildingId)
            {
                builtBuilding = &existingBuilding;
                break;
            }
        }
    }

    int filledJobs = 0;

    if (builtBuilding)
    {
        for (const BuildingTier& existingTier : builtBuilding->tiers)
        {
            filledJobs += existingTier.employedJobs;
        }
    }

    reason =
        std::string("Built ") +
        (
            builtBuilding
                ? buildingDisplayName(*builtBuilding)
                : buildingDisplayName(building)
        ) +
        ". Filled " +
        std::to_string(
            std::min(
                filledJobs,
                profile.jobCapacity
            )
        ) +
        " / " +
        std::to_string(profile.jobCapacity) +
        " jobs. Spent " +
        std::to_string(kBuildingPrestigeCost) +
        " Prestige.";

    return true;
}