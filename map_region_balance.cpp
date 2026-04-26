// File: map_region_balance.cpp
// Commit: Add modular region ownership balancing with minimum and maximum tile targets before land carving

#include "map_internal.h"
#include <array>
#include <limits>
#include <vector>

namespace
{
    constexpr std::array<Region, 15> kBalancedRegions = {
        Region::scandinavia,
        Region::france,
        Region::british_isles,
        Region::italy,
        Region::south_germany,
        Region::lowlands,
        Region::north_germany,
        Region::poland,
        Region::baltic,
        Region::russia,
        Region::pontic_steppe,
        Region::danubia,
        Region::balkans,
        Region::iberia,
        Region::anatolia
    };

    int regionIndex(Region region)
    {
        for (size_t i = 0; i < kBalancedRegions.size(); ++i)
        {
            if (kBalancedRegions[i] == region)
            {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    std::vector<int> countRegionTiles(const MapState& state)
    {
        std::vector<int> counts(kBalancedRegions.size(), 0);

        for (Region region : state.owner)
        {
            const int idx = regionIndex(region);
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

        for (const auto& seed : seeds)
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

        for (const auto& seed : seeds)
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

                const int targetScore = seedDistanceScore(x, y, *targetSeed);
                if (targetScore < 36)
                {
                    continue;
                }

                Region bestNeighborRegion = Region::none;
                int bestNeighborScore = std::numeric_limits<int>::max();

                for (const auto& d : kDirs)
                {
                    const int nx = x + d[0];
                    const int ny = y + d[1];

                    if (nx < 0 || ny < 0 || nx >= width || ny >= height)
                    {
                        continue;
                    }

                    const Region neighbor = state.owner[static_cast<size_t>(ny * width + nx)];
                    if (neighbor == target || neighbor == Region::none)
                    {
                        continue;
                    }

                    for (const auto& seed : seeds)
                    {
                        if (seed.region != neighbor)
                        {
                            continue;
                        }

                        const int neighborScore = seedDistanceScore(x, y, seed);
                        if (neighborScore < bestNeighborScore)
                        {
                            bestNeighborScore = neighborScore;
                            bestNeighborRegion = neighbor;
                        }

                        break;
                    }
                }

                if (bestNeighborRegion == Region::none)
                {
                    continue;
                }

                if (bestNeighborScore < bestReplacementScore)
                {
                    bestReplacementScore = bestNeighborScore;
                    bestIndex = idx;
                    replacement = bestNeighborRegion;
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

void balanceRegionOwnership(MapState& state, int width, int height)
{
    const int totalTiles = width * height;
    const int regionCount = static_cast<int>(kBalancedRegions.size());
    const int targetTilesPerRegion = totalTiles / regionCount;

    const int minTilesPerRegion = std::max(24, (targetTilesPerRegion * 60) / 100);
    const int maxTilesPerRegion = std::max(minTilesPerRegion + 8, (targetTilesPerRegion * 150) / 100);

    for (int pass = 0; pass < 10; ++pass)
    {
        std::vector<int> regionCounts = countRegionTiles(state);
        bool changed = false;

        for (size_t i = 0; i < kBalancedRegions.size(); ++i)
        {
            const Region region = kBalancedRegions[i];

            while (regionCounts[i] < minTilesPerRegion)
            {
                if (!growOneTile(state, state.seeds, width, height, region))
                {
                    break;
                }

                ++regionCounts[i];
                changed = true;
                regionCounts = countRegionTiles(state);
            }
        }

        for (size_t i = 0; i < kBalancedRegions.size(); ++i)
        {
            const Region region = kBalancedRegions[i];

            while (regionCounts[i] > maxTilesPerRegion)
            {
                if (!trimOneTile(state, state.seeds, width, height, region))
                {
                    break;
                }

                --regionCounts[i];
                changed = true;
                regionCounts = countRegionTiles(state);
            }
        }

        if (!changed)
        {
            break;
        }
    }
}