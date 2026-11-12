// File: map_region_layout_bounds.cpp
// Commit: Apply horizontal wraparound to region tile bound enforcement

#include "map_region_layout_internal.h"

#include <cstdint>
#include <limits>
#include <vector>

namespace
{
    bool canTrimRegionTile(const MapState& state, const Map& map, int x, int y, Region region)
    {
        if (state.owner[static_cast<size_t>(regionTileIndexWrapped(map.width(), x, y))] != region)
        {
            return false;
        }

        const int sameNeighbors =
            sameRegionNeighborCount(state, map.width(), map.height(), x, y, region);

        return sameNeighbors <= 2;
    }

    RegionCandidate findBestTrimReplacement(
        const MapState& state,
        const Map& map,
        Region region,
        int x,
        int y,
        std::uint32_t seedValue)
    {
        RegionCandidate best;
        const int width = map.width();
        const int height = map.height();

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
                continue;
            }

            const int neighborPlanIndex = regionPlanIndex(neighbor);
            if (neighborPlanIndex < 0)
            {
                continue;
            }

            const RegionPlan& neighborPlan = regionPlan(neighbor);

            const int score =
                regionCandidateScore(
                    state,
                    map,
                    state.seeds[static_cast<size_t>(neighborPlanIndex)],
                    neighborPlan,
                    x,
                    y,
                    seedValue
                );

            if (score > best.score)
            {
                best.score = score;
                best.index = static_cast<int>(neighbor);
            }
        }

        return best;
    }
}

void enforceRegionTileBounds(
    MapState& state,
    const Map& map,
    std::uint32_t seedValue)
{
    for (int pass = 0; pass < 12; ++pass)
    {
        std::vector<int> counts = countRegionTiles(state);
        bool changed = false;

        for (const RegionSeed& seed : state.seeds)
        {
            const Region region = seed.region;
            const int idx = regionPlanIndex(region);

            if (idx < 0)
            {
                continue;
            }

            while (counts[static_cast<size_t>(idx)] < minTileCount(map, region))
            {
                RegionCandidate best;

                for (int y = 1; y < map.height() - 1; ++y)
                {
                    for (int x = 0; x < map.width(); ++x)
                    {
                        const int tileIndex = regionTileIndexWrapped(map.width(), x, y);

                        const Region current =
                            state.owner[static_cast<size_t>(tileIndex)];

                        if (current == region || current == Region::none)
                        {
                            continue;
                        }

                        const int currentIdx = regionPlanIndex(current);

                        if (currentIdx >= 0 &&
                            counts[static_cast<size_t>(currentIdx)] <= minTileCount(map, current))
                        {
                            continue;
                        }

                        const int score =
                            regionCandidateScore(
                                state,
                                map,
                                seed,
                                regionPlan(region),
                                x,
                                y,
                                seedValue
                            );

                        if (score > best.score)
                        {
                            best.score = score;
                            best.index = tileIndex;
                        }
                    }
                }

                if (best.index < 0)
                {
                    break;
                }

                const Region previous =
                    state.owner[static_cast<size_t>(best.index)];

                state.owner[static_cast<size_t>(best.index)] = region;

                ++counts[static_cast<size_t>(idx)];

                const int previousIdx = regionPlanIndex(previous);

                if (previousIdx >= 0)
                {
                    --counts[static_cast<size_t>(previousIdx)];
                }

                changed = true;
            }
        }

        for (const RegionSeed& seed : state.seeds)
        {
            const Region region = seed.region;
            const int idx = regionPlanIndex(region);

            if (idx < 0)
            {
                continue;
            }

            while (counts[static_cast<size_t>(idx)] > maxTileCount(map, region))
            {
                int bestIndex = -1;
                int bestScore = std::numeric_limits<int>::min();
                Region bestReplacement = Region::none;

                for (int y = 1; y < map.height() - 1; ++y)
                {
                    for (int x = 0; x < map.width(); ++x)
                    {
                        const int tileIndex =
                            regionTileIndexWrapped(map.width(), x, y);

                        if (!canTrimRegionTile(state, map, x, y, region))
                        {
                            continue;
                        }

                        const RegionCandidate replacement =
                            findBestTrimReplacement(
                                state,
                                map,
                                region,
                                x,
                                y,
                                seedValue
                            );

                        if (replacement.score > bestScore)
                        {
                            bestScore = replacement.score;
                            bestIndex = tileIndex;
                            bestReplacement = static_cast<Region>(replacement.index);
                        }
                    }
                }

                if (bestIndex < 0 || bestReplacement == Region::none)
                {
                    break;
                }

                state.owner[static_cast<size_t>(bestIndex)] = bestReplacement;

                --counts[static_cast<size_t>(idx)];

                const int replacementIdx =
                    regionPlanIndex(bestReplacement);

                if (replacementIdx >= 0)
                {
                    ++counts[static_cast<size_t>(replacementIdx)];
                }

                changed = true;
            }
        }

        if (!changed)
        {
            break;
        }
    }
}