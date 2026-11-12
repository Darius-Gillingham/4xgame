// File: map_region_layout_adjacency.cpp
// Commit: Apply horizontal wraparound to final illegal adjacency repair

#include "map_region_layout_internal.h"

#include <algorithm>
#include <limits>
#include <vector>

namespace
{
    bool tileWouldHaveIllegalNeighbor(
        const MapState& state,
        int width,
        int height,
        int x,
        int y,
        Region region)
    {
        if (region == Region::none)
        {
            return false;
        }

        for (const auto& d : kDirs)
        {
            const int nx = wrapRegionX(x + d[0], width);
            const int ny = y + d[1];

            if (!inRegionYBounds(ny, height))
            {
                continue;
            }

            const Region neighbor =
                state.owner[static_cast<size_t>(regionTileIndexWrapped(width, nx, ny))];

            if (neighbor == Region::none || neighbor == region)
            {
                continue;
            }

            if (!regionsAreAdjacentInRules(region, neighbor))
            {
                return true;
            }
        }

        return false;
    }

    int legalNeighborSupport(
        const MapState& state,
        int width,
        int height,
        int x,
        int y,
        Region region)
    {
        int support = 0;

        for (const auto& d : kDirs)
        {
            const int nx = wrapRegionX(x + d[0], width);
            const int ny = y + d[1];

            if (!inRegionYBounds(ny, height))
            {
                continue;
            }

            const Region neighbor =
                state.owner[static_cast<size_t>(regionTileIndexWrapped(width, nx, ny))];

            if (neighbor == region)
            {
                support += 4;
            }
            else if (neighbor != Region::none && regionsAreAdjacentInRules(region, neighbor))
            {
                support += 1;
            }
        }

        return support;
    }

    Region bestLegalReplacementForTile(const MapState& state, const Map& map, int x, int y)
    {
        const int width = map.width();
        const int height = map.height();

        Region bestRegion = Region::none;
        int bestScore = std::numeric_limits<int>::min();

        for (const auto& d : kDirs)
        {
            const int nx = wrapRegionX(x + d[0], width);
            const int ny = y + d[1];

            if (!inRegionYBounds(ny, height))
            {
                continue;
            }

            const Region candidate =
                state.owner[static_cast<size_t>(regionTileIndexWrapped(width, nx, ny))];

            if (candidate == Region::none)
            {
                continue;
            }

            if (tileWouldHaveIllegalNeighbor(state, width, height, x, y, candidate))
            {
                continue;
            }

            const int candidatePlanIndex = regionPlanIndex(candidate);
            if (candidatePlanIndex < 0)
            {
                continue;
            }

            const RegionPlan& plan = regionPlan(candidate);
            const RegionSeed& seed = state.seeds[static_cast<size_t>(candidatePlanIndex)];

            int score = legalNeighborSupport(state, width, height, x, y, candidate) * 100;
            score -= anisotropicRegionDistanceScore(seed, plan, x, y) / 24;

            if (score > bestScore)
            {
                bestScore = score;
                bestRegion = candidate;
            }
        }

        return bestRegion;
    }

    bool repairOneIllegalAdjacencyPass(MapState& state, const Map& map)
    {
        const int width = map.width();
        const int height = map.height();

        std::vector<Region> next = state.owner;
        bool changed = false;

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const int idx = regionTileIndexWrapped(width, x, y);
                const Region region = state.owner[static_cast<size_t>(idx)];

                if (region == Region::none)
                {
                    continue;
                }

                if (!tileWouldHaveIllegalNeighbor(state, width, height, x, y, region))
                {
                    continue;
                }

                next[static_cast<size_t>(idx)] = bestLegalReplacementForTile(state, map, x, y);
                changed = true;
            }
        }

        if (changed)
        {
            state.owner.swap(next);
        }

        return changed;
    }
}

void enforceRegionAdjacencyRules(MapState& state, const Map& map)
{
    const int width = map.width();
    const int height = map.height();
    const int maxPasses = std::max(8, width + height);

    for (int pass = 0; pass < maxPasses; ++pass)
    {
        if (!repairOneIllegalAdjacencyPass(state, map))
        {
            break;
        }
    }

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const int idx = regionTileIndexWrapped(width, x, y);
            const Region region = state.owner[static_cast<size_t>(idx)];

            if (region == Region::none)
            {
                continue;
            }

            if (tileWouldHaveIllegalNeighbor(state, width, height, x, y, region))
            {
                state.owner[static_cast<size_t>(idx)] = Region::none;
            }
        }
    }
}