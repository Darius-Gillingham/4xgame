// File: map_region_layout_internal.h
// Commit: Add horizontal wraparound helpers for region layout generation

#pragma once

#include "map_internal.h"
#include "map_region_plan.h"

#include <cstdint>
#include <limits>
#include <random>
#include <vector>

struct RegionCandidate
{
    int index = -1;
    int score = std::numeric_limits<int>::min();
};

struct RegionPlanBounds
{
    int minX = 0;
    int maxX = 100;
    int minY = 0;
    int maxY = 100;
};

inline int wrapRegionX(int x, int width)
{
    if (width <= 0)
    {
        return 0;
    }

    int wrapped = x % width;

    if (wrapped < 0)
    {
        wrapped += width;
    }

    return wrapped;
}

inline bool inRegionYBounds(int y, int height)
{
    return y >= 0 && y < height;
}

inline int regionTileIndexWrapped(int width, int x, int y)
{
    return y * width + wrapRegionX(x, width);
}

int randInt(std::mt19937& rng, int minValue, int maxValue);
int distanceSq(int ax, int ay, int bx, int by);

std::uint32_t hashRegionValue(std::uint32_t value);
int regionNoiseValue(int x, int y, Region region, std::uint32_t seed);
RegionPlanBounds calculateRegionPlanBounds();
int normalizeRegionPlanCoordinate(int value, int minValue, int maxValue, int mapSize);

std::vector<int> countRegionTiles(const MapState& state);
int sameRegionNeighborCount(const MapState& state, int width, int height, int x, int y, Region region);
bool touchesIllegalRegionNeighbor(const MapState& state, int width, int height, int x, int y, Region region);
int adjacentRuleRegionNeighborCount(const MapState& state, int width, int height, int x, int y, Region region);
int anisotropicRegionDistanceScore(const RegionSeed& seed, const RegionPlan& plan, int x, int y);

int regionCandidateScore(
    const MapState& state,
    const Map& map,
    const RegionSeed& seed,
    const RegionPlan& plan,
    int x,
    int y,
    std::uint32_t seedValue
);

std::vector<RegionSeed> buildRegionSeeds(int width, int height, std::mt19937& rng);
void assignRegionSeedTiles(MapState& state, const Map& map);
void growRegionsEfficient(MapState& state, const Map& map, std::uint32_t seedValue);
void enforceRegionTileBounds(MapState& state, const Map& map, std::uint32_t seedValue);
void enforceRegionAdjacencyRules(MapState& state, const Map& map);