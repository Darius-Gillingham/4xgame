// File: building/province_state.cpp
// Commit: Add building levy state controls and reduce raised buildings to 80 percent production efficiency

#include "province.h"

#include <algorithm>
#include <vector>

namespace
{
    constexpr int kRaisedLevyEfficiencyPercent = 80;
    constexpr int kNormalEfficiencyPercent = 100;

    void addResourceAmount(
        std::vector<ResourceAmount>& amounts,
        ResourceAmount amount
    )
    {
        if (amount.amount == 0)
        {
            return;
        }

        for (ResourceAmount& existing : amounts)
        {
            if (existing.type == amount.type)
            {
                existing.amount += amount.amount;
                return;
            }
        }

        amounts.push_back(amount);
    }

    int resourceAmountForType(
        const std::vector<ResourceAmount>& amounts,
        ResourceType type
    )
    {
        for (const ResourceAmount& amount : amounts)
        {
            if (amount.type == type)
            {
                return amount.amount;
            }
        }

        return 0;
    }

    void addFoodProfile(
        FoodProfile& target,
        const FoodProfile& source,
        int amount
    )
    {
        if (amount == 0)
        {
            return;
        }

        target.fat += source.fat * amount;
        target.carbs += source.carbs * amount;
        target.protein += source.protein * amount;
        target.luxury += source.luxury * amount;
    }

    int employedJobsForBuilding(const Building& building)
    {
        int total = 0;

        for (const BuildingTier& tier : building.tiers)
        {
            total += tier.employedJobs;
        }

        return total;
    }

    bool buildingHasRaisedLevy(const Building& building)
    {
        for (const Levy& levy : building.levies)
        {
            if (levy.raised)
            {
                return true;
            }
        }

        return false;
    }

    const Building* findBuildingById(
        const Holding& holding,
        int buildingId
    )
    {
        for (const Building& building : holding.buildings)
        {
            if (building.id == buildingId)
            {
                return &building;
            }
        }

        return nullptr;
    }

    Building* findBuildingById(
        Holding& holding,
        int buildingId
    )
    {
        for (Building& building : holding.buildings)
        {
            if (building.id == buildingId)
            {
                return &building;
            }
        }

        return nullptr;
    }

    const Levy* findLevyBySlotIndex(
        const Building& building,
        int levySlotIndex
    )
    {
        for (const Levy& levy : building.levies)
        {
            if (levy.slotIndex == levySlotIndex)
            {
                return &levy;
            }
        }

        return nullptr;
    }

    Levy* findLevyBySlotIndex(
        Building& building,
        int levySlotIndex
    )
    {
        for (Levy& levy : building.levies)
        {
            if (levy.slotIndex == levySlotIndex)
            {
                return &levy;
            }
        }

        return nullptr;
    }

    BuildingTier* findTierByLevel(
        Building& building,
        int level
    )
    {
        for (BuildingTier& tier : building.tiers)
        {
            if (tier.level == level)
            {
                return &tier;
            }
        }

        return nullptr;
    }

    BuildingTier& findOrCreateTierByLevel(
        Building& building,
        int level
    )
    {
        if (BuildingTier* existingTier = findTierByLevel(building, level))
        {
            return *existingTier;
        }

        BuildingTier tier;
        tier.level = level;
        tier.count = 1;
        tier.employedJobs = 0;
        building.tiers.push_back(tier);

        return building.tiers.back();
    }
}

const char* popTypeName(PopType type)
{
    switch (type)
    {
        case PopType::farmer: return "Farmer";
        case PopType::laborer: return "Laborer";
        case PopType::artisan: return "Artisan";
        case PopType::merchant: return "Merchant";
        case PopType::clergy: return "Clergy";
        case PopType::noble: return "Noble";
        case PopType::tribesman: return "Tribesman";
        default: return "Unknown";
    }
}

Province::Province()
    : nextHoldingId_(1),
      nextPopId_(1),
      nextBuildingId_(1)
{
    Pop tribesmanPop;
    tribesmanPop.id = nextPopId_++;
    tribesmanPop.type = PopType::tribesman;
    tribesmanPop.population = 30;
    tribesmanPop.assignedHoldingId = -1;
    tribesmanPop.assignedBuildingId = -1;
    pops_.push_back(tribesmanPop);
}

int Province::totalPopulation() const
{
    int total = 0;

    for (const Pop& pop : pops_)
    {
        total += pop.population;
    }

    return total;
}

int Province::unassignedPopulation() const
{
    int total = 0;

    for (const Pop& pop : pops_)
    {
        if (pop.assignedHoldingId < 0)
        {
            total += pop.population;
        }
    }

    return total;
}

int Province::employedPopulation() const
{
    int total = 0;

    for (const Holding& holding : holdings_)
    {
        for (const Building& building : holding.buildings)
        {
            total += employedJobsForBuilding(building);
        }
    }

    return total;
}

int Province::availableWorkerPopulation() const
{
    int total = 0;

    for (const Pop& pop : pops_)
    {
        if (pop.assignedBuildingId < 0)
        {
            total += pop.population;
        }
    }

    return total;
}

ProvinceResourceSummary Province::resourceSummary() const
{
    ProvinceResourceSummary summary;

    for (const Holding& holding : holdings_)
    {
        for (const Building& building : holding.buildings)
        {
            const BuildingWorkProfile profile =
                workProfileForBuilding(building.type);

            const int rawEmployedJobs =
                employedJobsForBuilding(building);

            if (
                profile.jobCapacity <= 0 ||
                rawEmployedJobs <= 0
            )
            {
                continue;
            }

            const int employedJobs =
                std::min(
                    rawEmployedJobs,
                    profile.jobCapacity
                );

            const int efficiencyPercent =
                buildingHasRaisedLevy(building)
                    ? kRaisedLevyEfficiencyPercent
                    : kNormalEfficiencyPercent;

            for (const ResourceAmount& input : profile.inputs)
            {
                addResourceAmount(
                    summary.inputs,
                    {
                        input.type,
                        (input.amount * employedJobs) /
                            profile.jobCapacity
                    }
                );
            }

            for (const ResourceAmount& output : profile.outputs)
            {
                const int employedOutput =
                    (output.amount * employedJobs) /
                    profile.jobCapacity;

                addResourceAmount(
                    summary.outputs,
                    {
                        output.type,
                        (employedOutput * efficiencyPercent) /
                            kNormalEfficiencyPercent
                    }
                );
            }
        }
    }

    for (const ResourceAmount& output : summary.outputs)
    {
        const int inputAmount =
            resourceAmountForType(
                summary.inputs,
                output.type
            );

        addResourceAmount(
            summary.net,
            {
                output.type,
                output.amount - inputAmount
            }
        );
    }

    for (const ResourceAmount& input : summary.inputs)
    {
        const int outputAmount =
            resourceAmountForType(
                summary.outputs,
                input.type
            );

        if (outputAmount == 0)
        {
            addResourceAmount(
                summary.net,
                {
                    input.type,
                    -input.amount
                }
            );
        }
    }

    return summary;
}

ProvinceFoodSummary Province::foodSummary() const
{
    ProvinceFoodSummary summary;

    const ProvinceResourceSummary resources =
        resourceSummary();

    for (const ResourceAmount& input : resources.inputs)
    {
        if (!isFoodResource(input.type))
        {
            continue;
        }

        addFoodProfile(
            summary.inputs,
            foodProfileForResource(input.type),
            input.amount
        );
    }

    for (const ResourceAmount& output : resources.outputs)
    {
        if (!isFoodResource(output.type))
        {
            continue;
        }

        addFoodProfile(
            summary.outputs,
            foodProfileForResource(output.type),
            output.amount
        );
    }

    summary.net.fat =
        summary.outputs.fat -
        summary.inputs.fat;

    summary.net.carbs =
        summary.outputs.carbs -
        summary.inputs.carbs;

    summary.net.protein =
        summary.outputs.protein -
        summary.inputs.protein;

    summary.net.luxury =
        summary.outputs.luxury -
        summary.inputs.luxury;

    return summary;
}

const std::vector<Pop>& Province::pops() const
{
    return pops_;
}

ProvinceSaveData Province::saveData() const
{
    ProvinceSaveData data;
    data.nextHoldingId = nextHoldingId_;
    data.nextPopId = nextPopId_;
    data.nextBuildingId = nextBuildingId_;
    data.prestige = prestige_;
    data.pops = pops_;
    data.holdings = holdings_;

    return data;
}

void Province::loadData(const ProvinceSaveData& data)
{
    nextHoldingId_ = data.nextHoldingId;
    nextPopId_ = data.nextPopId;
    nextBuildingId_ = data.nextBuildingId;
    prestige_ = data.prestige;
    pops_ = data.pops;
    holdings_ = data.holdings;
}

bool Province::assignWorkersToBuilding(
    int holdingId,
    int buildingId,
    int workerCount,
    std::string& reason
)
{
    if (workerCount <= 0)
    {
        reason = "Worker count must be greater than 0.";
        return false;
    }

    Holding* holding =
        getHoldingById(holdingId);

    if (!holding)
    {
        reason = "Holding not found.";
        return false;
    }

    Building* selectedBuilding =
        findBuildingById(
            *holding,
            buildingId
        );

    if (!selectedBuilding)
    {
        reason = "Building not found.";
        return false;
    }

    const BuildingWorkProfile profile =
        workProfileForBuilding(
            selectedBuilding->type
        );

    const int currentEmployedJobs =
        employedJobsForBuilding(
            *selectedBuilding
        );

    const int openJobs =
        std::max(
            0,
            profile.jobCapacity -
                currentEmployedJobs
        );

    if (openJobs <= 0)
    {
        reason = "No open jobs.";
        return false;
    }

    const int availableWorkers =
        availableWorkerPopulation();

    if (availableWorkers <= 0)
    {
        reason = "No available workers.";
        return false;
    }

    int workersToAssign =
        std::min(
            workerCount,
            openJobs
        );

    workersToAssign =
        std::min(
            workersToAssign,
            availableWorkers
        );

    std::vector<Pop> assignedPops;
    int remainingWorkers = workersToAssign;

    for (Pop& pop : pops_)
    {
        if (remainingWorkers <= 0)
        {
            break;
        }

        if (
            pop.population <= 0 ||
            pop.assignedBuildingId >= 0
        )
        {
            continue;
        }

        const int takenWorkers =
            std::min(
                pop.population,
                remainingWorkers
            );

        pop.population -= takenWorkers;
        remainingWorkers -= takenWorkers;

        Pop assignedPop;
        assignedPop.id = nextPopId_++;
        assignedPop.type = pop.type;
        assignedPop.population = takenWorkers;
        assignedPop.assignedHoldingId = holdingId;
        assignedPop.assignedBuildingId = buildingId;
        assignedPops.push_back(assignedPop);
    }

    const int assignedWorkerCount =
        workersToAssign -
        remainingWorkers;

    if (assignedWorkerCount <= 0)
    {
        reason = "No workers were assigned.";
        return false;
    }

    for (const Pop& assignedPop : assignedPops)
    {
        pops_.push_back(assignedPop);
    }

    BuildingTier& levelOneTier =
        findOrCreateTierByLevel(
            *selectedBuilding,
            1
        );

    levelOneTier.employedJobs +=
        assignedWorkerCount;

    reason =
        "Assigned " +
        std::to_string(assignedWorkerCount) +
        " workers to " +
        buildingDisplayName(*selectedBuilding) +
        " tier 1.";

    return true;
}

int Province::raisedLevyCountForBuilding(
    int holdingId,
    int buildingId
) const
{
    const Holding* holding =
        getHoldingById(holdingId);

    if (!holding)
    {
        return 0;
    }

    const Building* building =
        findBuildingById(
            *holding,
            buildingId
        );

    if (!building)
    {
        return 0;
    }

    int raisedCount = 0;

    for (const Levy& levy : building->levies)
    {
        if (levy.raised)
        {
            ++raisedCount;
        }
    }

    return raisedCount;
}

int Province::availableLevyCountForBuilding(
    int holdingId,
    int buildingId
) const
{
    const Holding* holding =
        getHoldingById(holdingId);

    if (!holding)
    {
        return 0;
    }

    const Building* building =
        findBuildingById(
            *holding,
            buildingId
        );

    if (!building)
    {
        return 0;
    }

    int availableCount = 0;

    for (const Levy& levy : building->levies)
    {
        if (!levy.raised)
        {
            ++availableCount;
        }
    }

    return availableCount;
}

bool Province::raiseLevyFromBuilding(
    int holdingId,
    int buildingId,
    int levySlotIndex,
    std::string& reason
)
{
    Holding* holding =
        getHoldingById(holdingId);

    if (!holding)
    {
        reason = "Holding not found.";
        return false;
    }

    Building* building =
        findBuildingById(
            *holding,
            buildingId
        );

    if (!building)
    {
        reason = "Building not found.";
        return false;
    }

    Levy* levy =
        findLevyBySlotIndex(
            *building,
            levySlotIndex
        );

    if (!levy)
    {
        reason = "Levy slot not found.";
        return false;
    }

    if (levy->raised)
    {
        reason =
            "Levy " +
            std::to_string(levySlotIndex + 1) +
            " is already raised.";

        return false;
    }

    levy->raised = true;

    reason =
        "Raised Levy " +
        std::to_string(levySlotIndex + 1) +
        " from " +
        buildingDisplayName(*building) +
        ". Building production is now operating at 80% efficiency.";

    return true;
}

bool Province::lowerLevyFromBuilding(
    int holdingId,
    int buildingId,
    int levySlotIndex,
    std::string& reason
)
{
    Holding* holding =
        getHoldingById(holdingId);

    if (!holding)
    {
        reason = "Holding not found.";
        return false;
    }

    Building* building =
        findBuildingById(
            *holding,
            buildingId
        );

    if (!building)
    {
        reason = "Building not found.";
        return false;
    }

    Levy* levy =
        findLevyBySlotIndex(
            *building,
            levySlotIndex
        );

    if (!levy)
    {
        reason = "Levy slot not found.";
        return false;
    }

    if (!levy->raised)
    {
        reason =
            "Levy " +
            std::to_string(levySlotIndex + 1) +
            " is not raised.";

        return false;
    }

    levy->raised = false;

    if (buildingHasRaisedLevy(*building))
    {
        reason =
            "Lowered Levy " +
            std::to_string(levySlotIndex + 1) +
            " from " +
            buildingDisplayName(*building) +
            ". Another levy remains raised, so building production remains at 80% efficiency.";
    }
    else
    {
        reason =
            "Lowered Levy " +
            std::to_string(levySlotIndex + 1) +
            " from " +
            buildingDisplayName(*building) +
            ". Building production has returned to normal efficiency.";
    }

    return true;
}

const std::vector<Holding>& Province::holdings() const
{
    return holdings_;
}

const Holding* Province::getHoldingById(int id) const
{
    for (const Holding& holding : holdings_)
    {
        if (holding.id == id)
        {
            return &holding;
        }
    }

    return nullptr;
}

Holding* Province::getHoldingById(int id)
{
    for (Holding& holding : holdings_)
    {
        if (holding.id == id)
        {
            return &holding;
        }
    }

    return nullptr;
}