// File: map_region_layout_growth.cpp
// Commit: Apply horizontal wraparound to complete frontier growth implementation

#include "map_region_layout_internal.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <vector>

namespace
{
    void pushFrontierTile(
        const MapState& state,
        const Map& map,
        std::vector<std::vector<int>>& frontiers,
        std::vector<std::vector<unsigned char>>& queued,
        Region region,
        int tileIndex)
    {
        const int regionIndex = regionPlanIndex(region);
        if (regionIndex < 0)
        {
            return;
        }

        if (tileIndex < 0 || tileIndex >= static_cast<int>(state.owner.size()))
        {
            return;
        }

        if (state.owner[static_cast<size_t>(tileIndex)] != Region::none)
        {
            return;
        }

        if (queued[static_cast<size_t>(regionIndex)][static_cast<size_t>(tileIndex)] != 0U)
        {
            return;
        }

        const int x = tileIndex % map.width();
        const int y = tileIndex / map.width();

        if (sameRegionNeighborCount(state, map.width(), map.height(), x, y, region) <= 0)
        {
            return;
        }

        queued[static_cast<size_t>(regionIndex)][static_cast<size_t>(tileIndex)] = 1U;
        frontiers[static_cast<size_t>(regionIndex)].push_back(tileIndex);
    }

    void pushNeighborFrontierTiles(
        const MapState& state,
        const Map& map,
        std::vector<std::vector<int>>& frontiers,
        std::vector<std::vector<unsigned char>>& queued,
        Region region,
        int tileIndex)
    {
        const int width = map.width();
        const int height = map.height();
        const int x = tileIndex % width;
        const int y = tileIndex / width;

        for (const auto& d : kDirs)
        {
            const int nx = wrapRegionX(x + d[0], width);
            const int ny = y + d[1];

            if (!inRegionYBounds(ny, height))
            {
                continue;
            }

            pushFrontierTile(state, map, frontiers, queued, region, regionTileIndexWrapped(width, nx, ny));
        }
    }

    void seedInitialFrontiers(
        const MapState& state,
        const Map& map,
        std::vector<std::vector<int>>& frontiers,
        std::vector<std::vector<unsigned char>>& queued)
    {
        for (const RegionSeed& seed : state.seeds)
        {
            if (!inBoundsInternal(map, seed.x, seed.y))
            {
                continue;
            }

            pushNeighborFrontierTiles(
                state,
                map,
                frontiers,
                queued,
                seed.region,
                regionTileIndexWrapped(map.width(), seed.x, seed.y)
            );
        }
    }

    std::vector<int> buildGrowthOrderFromCounts(const MapState& state, const Map& map, const std::vector<int>& counts)
    {
        std::vector<int> order;
        order.reserve(state.seeds.size());

        for (size_t i = 0; i < state.seeds.size(); ++i)
        {
            order.push_back(static_cast<int>(i));
        }

        std::sort(order.begin(), order.end(), [&](int left, int right)
        {
            const Region leftRegion = state.seeds[static_cast<size_t>(left)].region;
            const Region rightRegion = state.seeds[static_cast<size_t>(right)].region;

            const int leftIdx = regionPlanIndex(leftRegion);
            const int rightIdx = regionPlanIndex(rightRegion);

            const int leftTarget = desiredTileCount(map, leftRegion);
            const int rightTarget = desiredTileCount(map, rightRegion);

            const int leftFill = (leftIdx >= 0) ? counts[static_cast<size_t>(leftIdx)] : 0;
            const int rightFill = (rightIdx >= 0) ? counts[static_cast<size_t>(rightIdx)] : 0;

            const int leftRatio = (leftFill * 1000) / std::max(1, leftTarget);
            const int rightRatio = (rightFill * 1000) / std::max(1, rightTarget);

            if (leftRatio != rightRatio)
            {
                return leftRatio < rightRatio;
            }

            return left < right;
        });

        return order;
    }

    RegionCandidate findBestFrontierCandidateForRegion(
        const MapState& state,
        const Map& map,
        std::vector<std::vector<int>>& frontiers,
        std::vector<std::vector<unsigned char>>& queued,
        const RegionSeed& seed,
        std::uint32_t seedValue)
    {
        RegionCandidate best;
        const int regionIndex = regionPlanIndex(seed.region);

        if (regionIndex < 0)
        {
            return best;
        }

        std::vector<int>& frontier = frontiers[static_cast<size_t>(regionIndex)];
        const RegionPlan& plan = regionPlan(seed.region);
        const int width = map.width();

        size_t writeIndex = 0;

        for (size_t readIndex = 0; readIndex < frontier.size(); ++readIndex)
        {
            const int tileIndex = frontier[readIndex];

            if (tileIndex < 0 || tileIndex >= static_cast<int>(state.owner.size()))
            {
                continue;
            }

            if (state.owner[static_cast<size_t>(tileIndex)] != Region::none)
            {
                queued[static_cast<size_t>(regionIndex)][static_cast<size_t>(tileIndex)] = 0U;
                continue;
            }

            const int x = tileIndex % width;
            const int y = tileIndex / width;
            const int score = regionCandidateScore(state, map, seed, plan, x, y, seedValue);

            if (score == std::numeric_limits<int>::min())
            {
                frontier[writeIndex] = tileIndex;
                ++writeIndex;
                continue;
            }

            if (score > best.score)
            {
                best.score = score;
                best.index = tileIndex;
            }

            frontier[writeIndex] = tileIndex;
            ++writeIndex;
        }

        frontier.resize(writeIndex);

        return best;
    }

    Region findNearestRegionForUnassignedTile(const MapState& state, const Map& map, int x, int y, std::uint32_t seedValue)
    {
        int bestScore = std::numeric_limits<int>::min();
        Region bestRegion = Region::none;

        for (const RegionSeed& seed : state.seeds)
        {
            const RegionPlan& plan = regionPlan(seed.region);
            const int score =
                -anisotropicRegionDistanceScore(seed, plan, x, y) +
                regionNoiseValue(x, y, seed.region, seedValue) * 3;

            if (score > bestScore)
            {
                bestScore = score;
                bestRegion = seed.region;
            }
        }

        return bestRegion;
    }

    bool fillOneFallbackTile(
        MapState& state,
        const Map& map,
        std::vector<std::vector<int>>& frontiers,
        std::vector<std::vector<unsigned char>>& queued,
        std::uint32_t seedValue)
    {
        const int width = map.width();
        const int height = map.height();

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const int tileIndex = regionTileIndexWrapped(width, x, y);

                if (state.owner[static_cast<size_t>(tileIndex)] != Region::none)
                {
                    continue;
                }

                const Region region = findNearestRegionForUnassignedTile(state, map, x, y, seedValue);
                state.owner[static_cast<size_t>(tileIndex)] = region;

                pushNeighborFrontierTiles(state, map, frontiers, queued, region, tileIndex);

                for (const auto& d : kDirs)
                {
                    const int nx = wrapRegionX(x + d[0], width);
                    const int ny = y + d[1];

                    if (!inRegionYBounds(ny, height))
                    {
                        continue;
                    }

                    const int neighborIndex = regionTileIndexWrapped(width, nx, ny);
                    const Region neighbor = state.owner[static_cast<size_t>(neighborIndex)];

                    if (neighbor != Region::none)
                    {
                        pushNeighborFrontierTiles(state, map, frontiers, queued, neighbor, neighborIndex);
                    }
                }

                return true;
            }
        }

        return false;
    }
}

void assignRegionSeedTiles(MapState& state, const Map& map)
{
    std::fill(state.owner.begin(), state.owner.end(), Region::none);

    for (const RegionSeed& seed : state.seeds)
    {
        if (!inBoundsInternal(map, seed.x, seed.y))
        {
            continue;
        }

        state.owner[static_cast<size_t>(regionTileIndexWrapped(map.width(), seed.x, seed.y))] = seed.region;
    }
}

void growRegionsEfficient(MapState& state, const Map& map, std::uint32_t seedValue)
{
    const int totalTilesCount = map.width() * map.height();
    int assignedTiles = 0;

    for (Region region : state.owner)
    {
        if (region != Region::none)
        {
            ++assignedTiles;
        }
    }

    std::vector<int> counts = countRegionTiles(state);
    std::vector<std::vector<int>> frontiers(regionPlanCount());
    std::vector<std::vector<unsigned char>> queued(
        regionPlanCount(),
        std::vector<unsigned char>(state.owner.size(), 0U)
    );

    seedInitialFrontiers(state, map, frontiers, queued);

    while (assignedTiles < totalTilesCount)
    {
        const std::vector<int> order = buildGrowthOrderFromCounts(state, map, counts);
        bool placed = false;

        for (int orderIndex : order)
        {
            const RegionSeed& seed = state.seeds[static_cast<size_t>(orderIndex)];
            const int countIndex = regionPlanIndex(seed.region);

            if (countIndex < 0)
            {
                continue;
            }

            if (counts[static_cast<size_t>(countIndex)] >= maxTileCount(map, seed.region))
            {
                continue;
            }

            const RegionCandidate candidate = findBestFrontierCandidateForRegion(
                state,
                map,
                frontiers,
                queued,
                seed,
                seedValue
            );

            if (candidate.index < 0)
            {
                continue;
            }

            state.owner[static_cast<size_t>(candidate.index)] = seed.region;
            queued[static_cast<size_t>(countIndex)][static_cast<size_t>(candidate.index)] = 0U;

            ++counts[static_cast<size_t>(countIndex)];
            ++assignedTiles;

            pushNeighborFrontierTiles(state, map, frontiers, queued, seed.region, candidate.index);

            const int x = candidate.index % map.width();
            const int y = candidate.index / map.width();

            for (const auto& d : kDirs)
            {
                const int nx = wrapRegionX(x + d[0], map.width());
                const int ny = y + d[1];

                if (!inRegionYBounds(ny, map.height()))
                {
                    continue;
                }

                const int neighborIndex = regionTileIndexWrapped(map.width(), nx, ny);
                const Region neighbor = state.owner[static_cast<size_t>(neighborIndex)];

                if (neighbor != Region::none)
                {
                    pushNeighborFrontierTiles(state, map, frontiers, queued, neighbor, neighborIndex);
                }
            }

            placed = true;
            break;
        }

        if (placed)
        {
            continue;
        }

        if (!fillOneFallbackTile(state, map, frontiers, queued, seedValue))
        {
            break;
        }

        counts = countRegionTiles(state);
        ++assignedTiles;
    }
}