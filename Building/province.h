// File: province.h
// Commit: Add province state, building construction rules, and attachment query helpers

#pragma once

#include "building.h"
#include <string>
#include <vector>

struct AttachmentCounts
{
    int directSmallholders = 0;
    int estates = 0;
};

class Province
{
public:
    Province();

    int totalPeasants() const;
    int freePeasants() const;

    bool canBuildRoot(BuildingType type, std::string& reason) const;
    bool buildRoot(BuildingType type, std::string& reason);

    bool canBuildSmallholderAttachment(int parentId, std::string& reason) const;
    bool buildSmallholderAttachment(int parentId, std::string& reason);

    bool canBuildEstateAttachment(int parentId, std::string& reason) const;
    bool buildEstateAttachment(int parentId, std::string& reason);

    const std::vector<Building>& buildings() const;
    const Building* getBuildingById(int id) const;
    Building* getBuildingById(int id);

    std::vector<int> rootBuildingIds() const;
    std::vector<int> directSmallholderIdsForParent(int parentId) const;
    std::vector<int> estateIdsForParent(int parentId) const;

    int directSmallholderSlotCapacityForBuilding(int buildingId) const;
    int estateSlotCapacityForBuilding(int buildingId) const;

    AttachmentCounts attachmentCountsForParent(int parentId) const;

private:
    int nextBuildingId_;
    int totalPeasants_;
    std::vector<Building> buildings_;

    bool hasCapitalType(BuildingType type) const;
    int buildAttached(BuildingType type, int parentId);
};