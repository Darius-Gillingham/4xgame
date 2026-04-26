// File: map_regions.cpp
// Commit: Replace circular Voronoi region ownership with contiguous irregular seeded growth and per-region tile bounds

#include "map_internal.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <vector>

int randInt(std::mt19937& rng, int minValue, int maxValue)
{
    return std::uniform_int_distribution<int>(minValue, maxValue)(rng);
}

int distanceSq(int ax, int ay, int bx, int by)
{
    const int dx = ax - bx;
    const int dy = ay - by;
    return dx * dx + dy * dy;
}

namespace
{
    struct RegionPlan
    {
        Region region;
        bool isBritain;
        int xPercent;
        int yPercent;
        int weight;
        int stretchX;
        int stretchY;
        int driftX;
        int driftY;
        std::array<Region, 5> neighbors;
        int neighborCount;
    };

    constexpr std::array<RegionPlan, 15> kRegionPlans = {{
        {Region::british_isles, true, 15, 28, 7,  8, 12, -2,  0, {Region::none, Region::none, Region::none, Region::none, Region::none}, 0},
        {Region::iberia,        false, 19, 78, 9, 11,  8, -2,  2, {Region::france, Region::none, Region::none, Region::none, Region::none}, 1},
        {Region::france,        false, 29, 56, 10, 12, 10,  0,  0, {Region::iberia, Region::italy, Region::south_germany, Region::lowlands, Region::none}, 4},
        {Region::lowlands,      false, 34, 43, 5,  6,  9,  1, -1, {Region::france, Region::south_germany, Region::north_germany, Region::none, Region::none}, 3},
        {Region::italy,         false, 47, 75, 7,  7, 12,  1,  2, {Region::france, Region::south_germany, Region::none, Region::none, Region::none}, 2},
        {Region::south_germany, false, 45, 57, 9, 11, 10,  1,  1, {Region::france, Region::lowlands, Region::italy, Region::north_germany, Region::danubia}, 5},
        {Region::north_germany, false, 48, 36, 8, 12,  8,  2, -1, {Region::lowlands, Region::south_germany, Region::scandinavia, Region::baltic, Region::poland}, 5},
        {Region::scandinavia,   false, 49, 18, 9, 10, 14,  0, -2, {Region::north_germany, Region::russia, Region::none, Region::none, Region::none}, 2},
        {Region::baltic,        false, 63, 34, 5,  9,  8,  1,  0, {Region::north_germany, Region::russia, Region::poland, Region::none, Region::none}, 3},
        {Region::danubia,       false, 60, 59, 7, 10,  8,  1,  1, {Region::south_germany, Region::poland, Region::balkans, Region::pontic_steppe, Region::none}, 4},
        {Region::poland,        false, 62, 45, 8, 10,  9,  1,  0, {Region::north_germany, Region::danubia, Region::baltic, Region::pontic_steppe, Region::russia}, 5},
        {Region::russia,        false, 79, 28, 13, 15, 12,  2, -1, {Region::baltic, Region::poland, Region::scandinavia, Region::pontic_steppe, Region::none}, 4},
        {Region::balkans,       false, 69, 69, 5,  8,  9,  1,  1, {Region::danubia, Region::none, Region::none, Region::none, Region::none}, 1},
        {Region::pontic_steppe, false, 79, 57, 10, 14,  8,  2,  0, {Region::danubia, Region::russia, Region::poland, Region::anatolia, Region::none}, 4},
        {Region::anatolia,      false, 89, 70, 7, 11,  9,  2,  1, {Region::pontic_steppe, Region::none, Region::none, Region::none, Region::none}, 1}
    }};

    struct Candidate
    {
        int index = -1;
        int score = std::numeric_limits<int>::min();
    };

    int regionPlanIndex(Region region)
    {
        for (size_t i = 0; i < kRegionPlans.size(); ++i)
        {
            if (kRegionPlans[i].region == region)
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
            return kRegionPlans[static_cast<size_t>(idx)];
        }

        return kRegionPlans.front();
    }

    bool regionsAreAdjacentInRules(Region a, Region b)
    {
        if (a == Region::none || b == Region::none || a == b)
        {
            return false;
        }

        const RegionPlan& planA = regionPlan(a);
        for (int i = 0; i < planA.neighborCount; ++i)
        {
            if (planA.neighbors[static_cast<size_t>(i)] == b)
            {
                return true;
            }
        }

        const RegionPlan& planB = regionPlan(b);
        for (int i = 0; i < planB.neighborCount; ++i)
        {
            if (planB.neighbors[static_cast<size_t>(i)] == a)
            {
                return true;
            }
        }

        return false;
    }

    int totalWeight()
    {
        int sum = 0;
        for (const RegionPlan& plan : kRegionPlans)
        {
            sum += plan.weight;
        }

        return std::max(1, sum);
    }

    int desiredTileCount(const Map& map, Region region)
    {
        const int totalTilesCount = map.width() * map.height();
        const RegionPlan& plan = regionPlan(region);
        return std::max(18, (totalTilesCount * plan.weight) / totalWeight());
    }

    int minTileCount(const Map& map, Region region)
    {
        return std::max(14, (desiredTileCount(map, region) * 82) / 100);
    }

    int maxTileCount(const Map& map, Region region)
    {
        return std::max(minTileCount(map, region) + 8, (desiredTileCount(map, region) * 120) / 100);
    }

    std::uint32_t hash32(std::uint32_t value)
    {
        value ^= value >> 16;
        value *= 0x7feb352dU;
        value ^= value >> 15;
        value *= 0x846ca68bU;
        value ^= value >> 16;
        return value;
    }

    int noiseValue(int x, int y, Region region, std::uint32_t seed)
    {
        std::uint32_t value = seed;
        value ^= static_cast<std::uint32_t>(x * 73856093);
        value ^= static_cast<std::uint32_t>(y * 19349663);
        value ^= static_cast<std::uint32_t>(static_cast<int>(region) * 83492791);
        value = hash32(value);
        return static_cast<int>(value % 101U) - 50;
    }

    std::vector<int> countRegionTiles(const MapState& state)
    {
        std::vector<int> counts(kRegionPlans.size(), 0);

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

    int sameNeighborCount(const MapState& state, int width, int height, int x, int y, Region region)
    {
        int count = 0;

        for (const auto& d : kDirs)
        {
            const int nx = x + d[0];
            const int ny = y + d[1];
            if (nx < 0 || ny < 0 || nx >= width || ny >= height)
            {
                continue;
            }

            if (state.owner[static_cast<size_t>(ny * width + nx)] == region)
            {
                ++count;
            }
        }

        return count;
    }

    bool touchesIllegalNeighbor(const MapState& state, int width, int height, int x, int y, Region region)
    {
        for (const auto& d : kDirs)
        {
            const int nx = x + d[0];
            const int ny = y + d[1];
            if (nx < 0 || ny < 0 || nx >= width || ny >= height)
            {
                continue;
            }

            const Region neighbor = state.owner[static_cast<size_t>(ny * width + nx)];
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

    int adjacentRuleNeighborCount(const MapState& state, int width, int height, int x, int y, Region region)
    {
        int count = 0;

        for (const auto& d : kDirs)
        {
            const int nx = x + d[0];
            const int ny = y + d[1];
            if (nx < 0 || ny < 0 || nx >= width || ny >= height)
            {
                continue;
            }

            const Region neighbor = state.owner[static_cast<size_t>(ny * width + nx)];
            if (neighbor != Region::none && neighbor != region && regionsAreAdjacentInRules(region, neighbor))
            {
                ++count;
            }
        }

        return count;
    }

    std::vector<RegionSeed> buildSeeds(int width, int height, std::mt19937& rng)
    {
        std::vector<RegionSeed> seeds;
        seeds.reserve(kRegionPlans.size());

        for (const RegionPlan& plan : kRegionPlans)
        {
            const int baseX = (width * plan.xPercent) / 100;
            const int baseY = (height * plan.yPercent) / 100;
            const int jitterX = std::max(1, width / 45);
            const int jitterY = std::max(1, height / 45);

            RegionSeed seed{};
            seed.region = plan.region;
            seed.isBritain = plan.isBritain;
            seed.x = std::clamp(baseX + randInt(rng, -jitterX, jitterX), 2, std::max(2, width - 3));
            seed.y = std::clamp(baseY + randInt(rng, -jitterY, jitterY), 2, std::max(2, height - 3));
            seeds.push_back(seed);
        }

        for (int pass = 0; pass < 24; ++pass)
        {
            for (size_t i = 0; i < seeds.size(); ++i)
            {
                for (size_t j = i + 1; j < seeds.size(); ++j)
                {
                    if (distanceSq(seeds[i].x, seeds[i].y, seeds[j].x, seeds[j].y) >= 64)
                    {
                        continue;
                    }

                    const int pushX = (seeds[j].x >= seeds[i].x) ? 1 : -1;
                    const int pushY = (seeds[j].y >= seeds[i].y) ? 1 : -1;

                    seeds[j].x = std::clamp(seeds[j].x + pushX, 2, std::max(2, width - 3));
                    seeds[j].y = std::clamp(seeds[j].y + pushY, 2, std::max(2, height - 3));
                }
            }
        }

        return seeds;
    }

    int anisotropicDistanceScore(const RegionSeed& seed, const RegionPlan& plan, int x, int y)
    {
        const int dx = x - seed.x;
        const int dy = y - seed.y;

        const int stretchX = std::max(1, plan.stretchX);
        const int stretchY = std::max(1, plan.stretchY);

        const int ax = (dx * dx * 100) / stretchX;
        const int ay = (dy * dy * 100) / stretchY;

        const int drift = (dx * plan.driftX * 8) + (dy * plan.driftY * 8);
        return ax + ay - drift;
    }

    int candidateScore(
        const MapState& state,
        const Map& map,
        const RegionSeed& seed,
        const RegionPlan& plan,
        int x,
        int y,
        std::uint32_t seedValue)
    {
        const int width = map.width();
        const int height = map.height();

        const int sameNeighbors = sameNeighborCount(state, width, height, x, y, plan.region);
        if (sameNeighbors <= 0)
        {
            return std::numeric_limits<int>::min();
        }

        if (touchesIllegalNeighbor(state, width, height, x, y, plan.region))
        {
            return std::numeric_limits<int>::min();
        }

        const int legalForeignNeighbors = adjacentRuleNeighborCount(state, width, height, x, y, plan.region);
        const int distancePenalty = anisotropicDistanceScore(seed, plan, x, y);
        const int noise = noiseValue(x, y, plan.region, seedValue);

        int score = 0;
        score += sameNeighbors * 140;
        score += legalForeignNeighbors * 18;
        score -= distancePenalty / 18;
        score += noise * 6;

        if (sameNeighbors == 1)
        {
            score += 36;
        }
        else if (sameNeighbors == 2)
        {
            score += 24;
        }
        else if (sameNeighbors >= 4)
        {
            score -= 80;
        }

        if (plan.isBritain)
        {
            score -= std::max(0, x - seed.x) * 8;
        }

        return score;
    }

    Candidate findBestCandidateForRegion(
        const MapState& state,
        const Map& map,
        const RegionSeed& seed,
        std::uint32_t seedValue)
    {
        Candidate best;
        const RegionPlan& plan = regionPlan(seed.region);
        const int width = map.width();
        const int height = map.height();

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const int idx = y * width + x;
                if (state.owner[static_cast<size_t>(idx)] != Region::none)
                {
                    continue;
                }

                const int score = candidateScore(state, map, seed, plan, x, y, seedValue);
                if (score > best.score)
                {
                    best.score = score;
                    best.index = idx;
                }
            }
        }

        return best;
    }

    void assignSeedTiles(MapState& state, const Map& map)
    {
        std::fill(state.owner.begin(), state.owner.end(), Region::none);

        for (const RegionSeed& seed : state.seeds)
        {
            if (!inBoundsInternal(map, seed.x, seed.y))
            {
                continue;
            }

            state.owner[static_cast<size_t>(seed.y * map.width() + seed.x)] = seed.region;
        }
    }

    std::vector<int> buildGrowthOrder(const MapState& state, const Map& map)
    {
        const std::vector<int> counts = countRegionTiles(state);
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

    void growRegions(MapState& state, const Map& map, std::uint32_t seedValue)
    {
        const int totalTilesCount = map.width() * map.height();

        while (true)
        {
            int assignedTiles = 0;
            for (Region region : state.owner)
            {
                if (region != Region::none)
                {
                    ++assignedTiles;
                }
            }

            if (assignedTiles >= totalTilesCount)
            {
                break;
            }

            const std::vector<int> counts = countRegionTiles(state);
            const std::vector<int> order = buildGrowthOrder(state, map);

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

                const Candidate candidate = findBestCandidateForRegion(state, map, seed, seedValue);
                if (candidate.index < 0)
                {
                    continue;
                }

                state.owner[static_cast<size_t>(candidate.index)] = seed.region;
                placed = true;
                break;
            }

            if (placed)
            {
                continue;
            }

            for (const RegionSeed& seed : state.seeds)
            {
                const Candidate candidate = findBestCandidateForRegion(state, map, seed, seedValue);
                if (candidate.index < 0)
                {
                    continue;
                }

                state.owner[static_cast<size_t>(candidate.index)] = seed.region;
                placed = true;
                break;
            }

            if (placed)
            {
                continue;
            }

            for (int y = 0; y < map.height(); ++y)
            {
                for (int x = 0; x < map.width(); ++x)
                {
                    const int idx = y * map.width() + x;
                    if (state.owner[static_cast<size_t>(idx)] != Region::none)
                    {
                        continue;
                    }

                    int bestScore = std::numeric_limits<int>::min();
                    Region bestRegion = Region::none;

                    for (const RegionSeed& seed : state.seeds)
                    {
                        const RegionPlan& plan = regionPlan(seed.region);
                        const int score =
                            -anisotropicDistanceScore(seed, plan, x, y) +
                            noiseValue(x, y, seed.region, seedValue) * 3;

                        if (score > bestScore)
                        {
                            bestScore = score;
                            bestRegion = seed.region;
                        }
                    }

                    state.owner[static_cast<size_t>(idx)] = bestRegion;
                    placed = true;
                    break;
                }

                if (placed)
                {
                    break;
                }
            }

            if (!placed)
            {
                break;
            }
        }
    }

    bool canTrimTile(const MapState& state, const Map& map, int x, int y, Region region)
    {
        if (state.owner[static_cast<size_t>(y * map.width() + x)] != region)
        {
            return false;
        }

        const int sameNeighbors = sameNeighborCount(state, map.width(), map.height(), x, y, region);
        return sameNeighbors <= 2;
    }

    void enforceTileBounds(MapState& state, const Map& map, std::uint32_t seedValue)
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
                    Candidate best;
                    const int width = map.width();
                    const int height = map.height();

                    for (int y = 1; y < height - 1; ++y)
                    {
                        for (int x = 1; x < width - 1; ++x)
                        {
                            const int tileIndex = y * width + x;
                            const Region current = state.owner[static_cast<size_t>(tileIndex)];

                            if (current == region || current == Region::none)
                            {
                                continue;
                            }

                            const int currentIdx = regionPlanIndex(current);
                            if (currentIdx >= 0 && counts[static_cast<size_t>(currentIdx)] <= minTileCount(map, current))
                            {
                                continue;
                            }

                            const int score = candidateScore(state, map, seed, regionPlan(region), x, y, seedValue);
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

                    const Region previous = state.owner[static_cast<size_t>(best.index)];
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
                        for (int x = 1; x < map.width() - 1; ++x)
                        {
                            const int tileIndex = y * map.width() + x;
                            if (!canTrimTile(state, map, x, y, region))
                            {
                                continue;
                            }

                            for (const auto& d : kDirs)
                            {
                                const int nx = x + d[0];
                                const int ny = y + d[1];
                                const Region neighbor = state.owner[static_cast<size_t>(ny * map.width() + nx)];

                                if (neighbor == Region::none || neighbor == region)
                                {
                                    continue;
                                }

                                if (!regionsAreAdjacentInRules(region, neighbor))
                                {
                                    continue;
                                }

                                const RegionPlan& neighborPlan = regionPlan(neighbor);
                                int score =
                                    candidateScore(state, map, state.seeds[static_cast<size_t>(regionPlanIndex(neighbor))], neighborPlan, x, y, seedValue);

                                if (score > bestScore)
                                {
                                    bestScore = score;
                                    bestIndex = tileIndex;
                                    bestReplacement = neighbor;
                                }
                            }
                        }
                    }

                    if (bestIndex < 0 || bestReplacement == Region::none)
                    {
                        break;
                    }

                    state.owner[static_cast<size_t>(bestIndex)] = bestReplacement;
                    --counts[static_cast<size_t>(idx)];

                    const int replacementIdx = regionPlanIndex(bestReplacement);
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
}

MapState buildRegionLayout(const Map& map, std::uint32_t seed)
{
    std::mt19937 rng(seed);

    MapState state;
    state.seeds = buildSeeds(map.width(), map.height(), rng);
    state.owner.resize(static_cast<size_t>(map.width() * map.height()), Region::none);
    state.landMask.resize(static_cast<size_t>(map.width() * map.height()), false);

    assignSeedTiles(state, map);
    growRegions(state, map, seed);
    enforceTileBounds(state, map, seed);

    return state;
}