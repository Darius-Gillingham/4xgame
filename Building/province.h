// File: building/province.h
// Commit: Add province interfaces for raising and lowering generic building levies

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
    int assignedHoldingId = -1;
    int assignedBuildingId = -1;
};

struct AttachmentCounts
{
    int directSmallholders = 0;
    int estates = 0;
};

struct ProvinceResourceSummary
{
    std::vector<ResourceAmount> inputs;
    std::vector<ResourceAmount> outputs;
    std::vector<ResourceAmount> net;
};

struct ProvinceFoodSummary
{
    FoodProfile inputs;
    FoodProfile outputs;
    FoodProfile net;
};

struct ProvinceSaveData
{
    int nextHoldingId = 1;
    int nextPopId = 1;
    int nextBuildingId = 1;
    int prestige = 10000;
    std::vector<Pop> pops;
    std::vector<Holding> holdings;
};

class Province
{
public:
    Province();

    int totalPopulation() const;
    int unassignedPopulation() const;
    int employedPopulation() const;
    int availableWorkerPopulation() const;

    int prestige() const
    {
        return prestige_;
    }

    void addPrestige(int amount)
    {
        prestige_ += amount;
    }

    bool spendPrestige(int amount)
    {
        if (amount < 0 || prestige_ < amount)
        {
            return false;
        }

        prestige_ -= amount;
        return true;
    }

    ProvinceResourceSummary resourceSummary() const;
    ProvinceFoodSummary foodSummary() const;

    const std::vector<Pop>& pops() const;

    ProvinceSaveData saveData() const;
    void loadData(const ProvinceSaveData& data);

    bool assignWorkersToBuilding(
        int holdingId,
        int buildingId,
        int workerCount,
        std::string& reason
    );

    int raisedLevyCountForBuilding(
        int holdingId,
        int buildingId
    ) const;

    int availableLevyCountForBuilding(
        int holdingId,
        int buildingId
    ) const;

    bool raiseLevyFromBuilding(
        int holdingId,
        int buildingId,
        int levySlotIndex,
        std::string& reason
    );

    bool lowerLevyFromBuilding(
        int holdingId,
        int buildingId,
        int levySlotIndex,
        std::string& reason
    );

    bool canBuildRootHolding(
        HoldingType type,
        std::string& reason
    ) const;

    bool buildRootHolding(
        HoldingType type,
        std::string& reason
    );

    bool canBuildSmallholderAttachment(
        int parentId,
        std::string& reason
    ) const;

    bool buildSmallholderAttachment(
        int parentId,
        std::string& reason
    );

    bool canBuildEstateAttachment(
        int parentId,
        std::string& reason
    ) const;

    bool buildEstateAttachment(
        int parentId,
        std::string& reason
    );

    bool canBuildBuilding(
        int parentId,
        BuildingType type,
        std::string& reason
    ) const;

    bool buildBuilding(
        int parentId,
        BuildingType type,
        std::string& reason
    );

    const std::vector<Holding>& holdings() const;

    const Holding* getHoldingById(int id) const;
    Holding* getHoldingById(int id);

    std::vector<int> rootHoldingIds() const;
    std::vector<int> directSmallholderIdsForParent(int parentId) const;
    std::vector<int> estateIdsForParent(int parentId) const;

    int directSmallholderSlotCapacityForHolding(int holdingId) const;
    int estateSlotCapacityForHolding(int holdingId) const;
    int buildingSlotCapacityForHolding(int holdingId) const;

    AttachmentCounts attachmentCountsForParent(int parentId) const;

private:
    int nextHoldingId_;
    int nextPopId_;
    int nextBuildingId_;
    int prestige_ = 10000;

    std::vector<Pop> pops_;
    std::vector<Holding> holdings_;

    bool hasCapitalType(HoldingType type) const;
    int buildAttachedHolding(HoldingType type, int parentId);
};