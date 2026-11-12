// File: map_region_adjacency.cpp
// Commit: Add shared world region plan aggregation and adjacency helpers

#include "map_region_plan.h"

#include <algorithm>

const std::vector<RegionPlan>& allRegionPlans()
{
    static const std::vector<RegionPlan> plans = []
    {
        std::vector<RegionPlan> combined;

        combined.insert(combined.end(), europeRegionPlans().begin(), europeRegionPlans().end());
        combined.insert(combined.end(), asiaRegionPlans().begin(), asiaRegionPlans().end());
        combined.insert(combined.end(), africaRegionPlans().begin(), africaRegionPlans().end());
        combined.insert(combined.end(), americasRegionPlans().begin(), americasRegionPlans().end());
        combined.insert(combined.end(), oceaniaRegionPlans().begin(), oceaniaRegionPlans().end());

        return combined;
    }();

    return plans;
}

int regionPlanCount()
{
    return static_cast<int>(allRegionPlans().size());
}

int regionPlanIndex(Region region)
{
    const std::vector<RegionPlan>& plans = allRegionPlans();

    for (size_t i = 0; i < plans.size(); ++i)
    {
        if (plans[i].region == region)
        {
            return static_cast<int>(i);
        }
    }

    return -1;
}

const RegionPlan& regionPlan(Region region)
{
    const int idx = regionPlanIndex(region);

    if (idx >= 0)
    {
        return allRegionPlans()[static_cast<size_t>(idx)];
    }

    return allRegionPlans().front();
}

int totalRegionWeight()
{
    int sum = 0;

    for (const RegionPlan& plan : allRegionPlans())
    {
        sum += plan.weight;
    }

    return std::max(1, sum);
}

int desiredTileCount(const Map& map, Region region)
{
    const int totalTilesCount = map.width() * map.height();
    const RegionPlan& plan = regionPlan(region);

    return std::max(18, (totalTilesCount * plan.weight) / totalRegionWeight());
}

int minTileCount(const Map& map, Region region)
{
    return std::max(14, (desiredTileCount(map, region) * 82) / 100);
}

int maxTileCount(const Map& map, Region region)
{
    return std::max(minTileCount(map, region) + 8, (desiredTileCount(map, region) * 120) / 100);
}

bool regionsAreAdjacentInRules(Region a, Region b)
{
    if (a == Region::none || b == Region::none || a == b)
    {
        return false;
    }

    const int planAIndex = regionPlanIndex(a);
    if (planAIndex >= 0)
    {
        const RegionPlan& planA = allRegionPlans()[static_cast<size_t>(planAIndex)];

        for (int i = 0; i < planA.neighborCount; ++i)
        {
            if (planA.neighbors[static_cast<size_t>(i)] == b)
            {
                return true;
            }
        }
    }

    const int planBIndex = regionPlanIndex(b);
    if (planBIndex >= 0)
    {
        const RegionPlan& planB = allRegionPlans()[static_cast<size_t>(planBIndex)];

        for (int i = 0; i < planB.neighborCount; ++i)
        {
            if (planB.neighbors[static_cast<size_t>(i)] == a)
            {
                return true;
            }
        }
    }

    return false;
}