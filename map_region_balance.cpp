// File: map_region_balance.cpp
// Commit: Update region ownership balancing to use shared world region plan helpers

#include "map_internal.h"
#include "map_region_plan.h"

#include <limits>
#include <vector>

namespace
{
    std::vector<int> countRegionTiles(const MapState& state)
    {
        std::vector<int> counts(regionPlanCount(), 0);

        for (Region region : state.owner)
        {
            const int idx = regionPlanIndex(region);
            if (idx >= 0)
            {
                ++counts[static_cast<size_t>(idx)];
            }
        }

        return counts;
    }

    bool touchesRegion(const MapState& state, int width, int height, int x, int y, Region target)
    {
        for (const auto& d : kDirs)
        {
            const int nx = x + d[0];
            const int ny = y + d[1];

            if (nx < 0 || ny < 0 || nx >= width || ny >= height)
            {
                continue;
            }

            if (state.owner[static_cast<size_t>(ny * width + nx)] == target)
            {
                return true;
            }
        }

        return false;
    }

    int seedDistanceScore(int x, int y, const RegionSeed& seed)
    {
        return distanceSq(x, y, seed.x, seed.y);
    }

    bool growOneTile(MapState& state, const std::vector<RegionSeed>& seeds, int width, int height, Region target)
    {
        const RegionSeed* targetSeed = nullptr;

        for (const RegionSeed& seed : seeds)
        {
            if (seed.region == target)
            {
                targetSeed = &seed;
                break;
            }
        }

        if (!targetSeed)
        {
            return false;
        }

        int bestIndex = -1;
        int bestScore = std::numeric_limits<int>::max();

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const int idx = y * width + x;
                const Region current = state.owner[static_cast<size_t>(idx)];

                if (current == target || !touchesRegion(state, width, height, x, y, target))
                {
                    continue;
                }

                if (current != Region::none && !regionsAreAdjacentInRules(target, current))
                {
                    continue;
                }

                const int score = seedDistanceScore(x, y, *targetSeed);

                if (score < bestScore)
                {
                    bestScore = score;
                    bestIndex = idx;
                }
            }
        }

        if (bestIndex < 0)
        {
            return false;
        }

        state.owner[static_cast<size_t>(bestIndex)] = target;
        return true;
    }

    bool trimOneTile(MapState& state, const std::vector<RegionSeed>& seeds, int width, int height, Region target)
    {
        const RegionSeed* targetSeed = nullptr;

        for (const RegionSeed& seed : seeds)
        {
            if (seed.region == target)
            {
                targetSeed = &seed;
                break;
            }
        }

        if (!targetSeed)
        {
            return false;
        }

        int bestIndex = -1;
        int bestReplacementScore = std::numeric_limits<int>::max();
        Region replacement = Region::none;

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const int idx = y * width + x;

                if (state.owner[static_cast<size_t>(idx)] != target)
                {
                    continue;
                }

                for (const auto& d : kDirs)
                {
                    const int nx = x + d[0];
                    const int ny = y + d[1];

                    if (nx < 0 || ny < 0 || nx >= width || ny >= height)
                    {
                        continue;
                    }

                    const Region neighbor = state.owner[static_cast<size_t>(ny * width + nx)];

                    if (neighbor == Region::none || neighbor == target)
                    {
                        continue;
                    }

                    if (!regionsAreAdjacentInRules(target, neighbor))
                    {
                        continue;
                    }

                    const int neighborPlanIndex = regionPlanIndex(neighbor);

                    if (neighborPlanIndex < 0 || neighborPlanIndex >= static_cast<int>(seeds.size()))
                    {
                        continue;
                    }

                    const RegionSeed& neighborSeed = seeds[static_cast<size_t>(neighborPlanIndex)];
                    const int score = seedDistanceScore(x, y, neighborSeed);

                    if (score < bestReplacementScore)
                    {
                        bestReplacementScore = score;
                        bestIndex = idx;
                        replacement = neighbor;
                    }
                }
            }
        }

        if (bestIndex < 0 || replacement == Region::none)
        {
            return false;
        }

        state.owner[static_cast<size_t>(bestIndex)] = replacement;
        return true;
    }
}

void balanceRegionOwnership(MapState& state, const Map& map)
{
    if (state.owner.empty() || state.seeds.empty())
    {
        return;
    }

    for (int pass = 0; pass < 20; ++pass)
    {
        bool changed = false;
        std::vector<int> counts = countRegionTiles(state);

        for (const RegionPlan& plan : allRegionPlans())
        {
            const int idx = regionPlanIndex(plan.region);

            if (idx < 0)
            {
                continue;
            }

            while (counts[static_cast<size_t>(idx)] < minTileCount(map, plan.region))
            {
                if (!growOneTile(state, state.seeds, map.width(), map.height(), plan.region))
                {
                    break;
                }

                counts = countRegionTiles(state);
                changed = true;
            }
        }

        for (const RegionPlan& plan : allRegionPlans())
        {
            const int idx = regionPlanIndex(plan.region);

            if (idx < 0)
            {
                continue;
            }

            while (counts[static_cast<size_t>(idx)] > maxTileCount(map, plan.region))
            {
                if (!trimOneTile(state, state.seeds, map.width(), map.height(), plan.region))
                {
                    break;
                }

                counts = countRegionTiles(state);
                changed = true;
            }
        }

        if (!changed)
        {
            break;
        }
    }
}      