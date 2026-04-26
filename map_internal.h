// File: map_internal.h
// Commit: Add shared land-shaping structures and declarations for Europe-style water-heavy map generation

#pragma once
#include "map.h"
#include <array>
#include <cstdint>
#include <random>
#include <vector>

struct RegionSeed
{
    Region region;
    int x;
    int y;
    bool isBritain;
};

struct MapState
{
    std::vector<RegionSeed> seeds;
    std::vector<Region> owner;
    std::vector<bool> landMask;
};

struct LandProfile
{
    int westCoastBase = 0;
    int northCoastBase = 0;
    int southCoastBase = 0;
    int eastOpenBias = 0;
};

struct BritainShape
{
    int centerX = 0;
    int centerY = 0;
    int radiusX = 0;
    int radiusY = 0;
    int channelX = 0;
    int channelWidth = 0;
};

inline constexpr int kDirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
inline constexpr int kDiagDirs[8][2] = { {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1} };

inline int indexInternal(const Map& map, int x, int y)
{
    return y * map.width() + x;
}

inline bool inBoundsInternal(const Map& map, int x, int y)
{
    return x >= 0 && y >= 0 && x < map.width() && y < map.height();
}

template <typename T>
inline T pickFrom(const std::vector<T>& values, std::mt19937& rng)
{
    return values[static_cast<size_t>(
        std::uniform_int_distribution<int>(0, static_cast<int>(values.size()) - 1)(rng)
    )];
}

int randInt(std::mt19937& rng, int minValue, int maxValue);
int distanceSq(int ax, int ay, int bx, int by);
Terrain pickTerrainForRegion(Region region, std::mt19937& rng);

MapState buildRegionLayout(const Map& map, std::uint32_t seed);

LandProfile buildLandProfile(const Map& map, std::mt19937& rng);
BritainShape buildBritainShape(const Map& map, const MapState& state, std::mt19937& rng);

void applyLandmask(Map& map, MapState& state);
void assignTerrainClusters(Map& map, std::uint32_t seed);
void smoothTerrainPasses(Map& map);

bool isInsideBritainShape(int x, int y, const BritainShape& britain);
bool isInsideMainlandEnvelope(int x, int y, int width, int height, const LandProfile& profile);
void carveCoastlineNoise(std::vector<bool>& mask, int width, int height, std::mt19937& rng);
void enforceBritishChannel(std::vector<bool>& mask, int width, int height, const BritainShape& britain);
void keepLargestMainland(std::vector<bool>& mask, int width, int height, const BritainShape& britain);