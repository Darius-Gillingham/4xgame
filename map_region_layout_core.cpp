// File: map_region_layout_core.cpp
// Commit: Apply horizontal wraparound to shared region neighbor scoring

#include "map_region_layout_internal.h"

#include <algorithm>
#include <cstdint>
#include <random>
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

std::uint32_t hashRegionValue(std::uint32_t value)
{
    value ^= value >> 16;
    value *= 0x7feb352dU;
    value ^= value >> 15;
    value *= 0x846ca68bU;
    value ^= value >> 16;
    return value;
}

int regionNoiseValue(int x, int y, Region region, std::uint32_t seed)
{
    std::uint32_t value = seed;
    value ^= static_cast<std::uint32_t>(wrapRegionX(x, 1000003) * 73856093);
    value ^= static_cast<std::uint32_t>(y * 19349663);
    value ^= static_cast<std::uint32_t>(static_cast<int>(region) * 83492791);
    value = hashRegionValue(value);
    return static_cast<int>(value % 101U) - 50;
}

RegionPlanBounds calculateRegionPlanBounds()
{
    RegionPlanBounds bounds{};

    if (allRegionPlans().empty())
    {
        return bounds;
    }

    bounds.minX = allRegionPlans().front().xPercent;
    bounds.maxX = allRegionPlans().front().xPercent;
    bounds.minY = allRegionPlans().front().yPercent;
    bounds.maxY = allRegionPlans().front().yPercent;

    for (const RegionPlan& plan : allRegionPlans())
    {
        bounds.minX = std::min(bounds.minX, plan.xPercent);
        bounds.maxX = std::max(bounds.maxX, plan.xPercent);
        bounds.minY = std::min(bounds.minY, plan.yPercent);
        bounds.maxY = std::max(bounds.maxY, plan.yPercent);
    }

    return bounds;
}

int normalizeRegionPlanCoordinate(int value, int minValue, int maxValue, int mapSize)
{
    const int safeMin = 2;
    const int safeMax = std::max(2, mapSize - 3);
    const int safeSpan = std::max(1, safeMax - safeMin);
    const int sourceSpan = std::max(1, maxValue - minValue);

    return safeMin + ((value - minValue) * safeSpan) / sourceSpan;
}

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

int sameRegionNeighborCount(const MapState& state, int width, int height, int x, int y, Region region)
{
    int count = 0;

    for (const auto& d : kDirs)
    {
        const int nx = x + d[0];
        const int ny = y + d[1];

        if (!inRegionYBounds(ny, height))
        {
            continue;
        }

        if (state.owner[static_cast<size_t>(regionTileIndexWrapped(width, nx, ny))] == region)
        {
            ++count;
        }
    }

    return count;
}

bool touchesIllegalRegionNeighbor(const MapState& state, int width, int height, int x, int y, Region region)
{
    for (const auto& d : kDirs)
    {
        const int nx = x + d[0];
        const int ny = y + d[1];

        if (!inRegionYBounds(ny, height))
        {
            continue;
        }

        const Region neighbor = state.owner[static_cast<size_t>(regionTileIndexWrapped(width, nx, ny))];

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

int adjacentRuleRegionNeighborCount(const MapState& state, int width, int height, int x, int y, Region region)
{
    int count = 0;

    for (const auto& d : kDirs)
    {
        const int nx = x + d[0];
        const int ny = y + d[1];

        if (!inRegionYBounds(ny, height))
        {
            continue;
        }

        const Region neighbor = state.owner[static_cast<size_t>(regionTileIndexWrapped(width, nx, ny))];

        if (neighbor != Region::none && neighbor != region && regionsAreAdjacentInRules(region, neighbor))
        {
            ++count;
        }
    }

    return count;
}

int anisotropicRegionDistanceScore(const RegionSeed& seed, const RegionPlan& plan, int x, int y)
{
    const int dxRaw = x - seed.x;
    const int dx = dxRaw;
    const int dy = y - seed.y;

    const int stretchX = std::max(1, plan.stretchX);
    const int stretchY = std::max(1, plan.stretchY);

    const int ax = (dx * dx * 100) / stretchX;
    const int ay = (dy * dy * 100) / stretchY;

    const int drift = (dx * plan.driftX * 8) + (dy * plan.driftY * 8);

    return ax + ay - drift;
}

int regionCandidateScore(
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

    const int sameNeighbors = sameRegionNeighborCount(state, width, height, x, y, plan.region);
    if (sameNeighbors <= 0)
    {
        return std::numeric_limits<int>::min();
    }

    if (touchesIllegalRegionNeighbor(state, width, height, x, y, plan.region))
    {
        return std::numeric_limits<int>::min();
    }

    const int legalForeignNeighbors = adjacentRuleRegionNeighborCount(state, width, height, x, y, plan.region);
    const int distancePenalty = anisotropicRegionDistanceScore(seed, plan, x, y);
    const int noise = regionNoiseValue(x, y, plan.region, seedValue);

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