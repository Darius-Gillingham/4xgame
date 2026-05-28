// File: province.h
// Commit: Replace vestigial peasant state with province pop structures and population query helpers

#pragma once

#include "building.h"
#include <string>
#include <vector>

enum class PopType
{
    farmer,
    laborer,
    artisan,
    merchant,
    clergy,
    noble,
    tribesman
};

const char* popTypeName(PopType type);

struct Pop
{
    int id = -1;
    PopType type = PopType::farmer;
    int population = 0;
    int assignedBuildingId = -1;
};

struct AttachmentCounts
{
    int directSmallholders = 0;
    int estates = 0;
};

class Province
{
public:
    Province();

    int totalPopulation() const;
    int unassignedPopulation() const;

    const std::vector<Pop>& pops() const;

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
    int nextPopId_;
    std::vector<Pop> pops_;
    std::vector<Building> buildings_;

    bool hasCapitalType(BuildingType type) const;
    int buildAttached(BuildingType type, int parentId);
};