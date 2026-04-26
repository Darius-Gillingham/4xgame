// File: map_land.cpp
// Commit: Remove fixed Europe silhouette and fake regional seas by deriving coastlines from edge-driven ocean growth and adjacency enforcement

#include "map_internal.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <queue>
#include <vector>

namespace
{
    struct OceanNode
    {
        int x;
        int y;
    };

    bool inBoundsRaw(int x, int y, int width, int height)
    {
        return x >= 0 && y >= 0 && x < width && y < height;
    }

    int indexRaw(int x, int y, int width)
    {
        return y * width + x;
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

    int tileNoise(int x, int y, std::uint32_t seed)
    {
        std::uint32_t value = seed;
        value ^= static_cast<std::uint32_t>(x * 73856093);
        value ^= static_cast<std::uint32_t>(y * 19349663);
        value = hash32(value);
        return static_cast<int>(value % 101U) - 50;
    }

    bool regionsAreAdjacentInRules(Region a, Region b)
    {
        if (a == Region::none || b == Region::none || a == b)
        {
            return false;
        }

        switch (a)
        {
            case Region::british_isles:
                return false;

            case Region::iberia:
                return b == Region::france;

            case Region::france:
                return b == Region::iberia || b == Region::italy || b == Region::south_germany || b == Region::lowlands;

            case Region::lowlands:
                return b == Region::france || b == Region::south_germany || b == Region::north_germany;

            case Region::italy:
                return b == Region::france || b == Region::south_germany;

            case Region::south_germany:
                return b == Region::france || b == Region::lowlands || b == Region::italy || b == Region::north_germany || b == Region::danubia;

            case Region::north_germany:
                return b == Region::lowlands || b == Region::south_germany || b == Region::scandinavia || b == Region::baltic || b == Region::poland;

            case Region::scandinavia:
                return b == Region::north_germany || b == Region::russia;

            case Region::baltic:
                return b == Region::north_germany || b == Region::russia || b == Region::poland;

            case Region::danubia:
                return b == Region::south_germany || b == Region::poland || b == Region::balkans || b == Region::pontic_steppe;

            case Region::poland:
                return b == Region::north_germany || b == Region::danubia || b == Region::baltic || b == Region::pontic_steppe || b == Region::russia;

            case Region::russia:
                return b == Region::baltic || b == Region::poland || b == Region::scandinavia || b == Region::pontic_steppe;

            case Region::balkans:
                return b == Region::danubia;

            case Region::pontic_steppe:
                return b == Region::danubia || b == Region::russia || b == Region::poland || b == Region::anatolia;

            case Region::anatolia:
                return b == Region::pontic_steppe;

            default:
                return false;
        }
    }

    int coastalNeed(Region region)
    {
        switch (region)
        {
            case Region::british_isles: return 95;
            case Region::iberia: return 75;
            case Region::france: return 68;
            case Region::lowlands: return 82;
            case Region::italy: return 78;
            case Region::north_germany: return 58;
            case Region::scandinavia: return 85;
            case Region::baltic: return 70;
            case Region::russia: return 42;
            case Region::balkans: return 55;
            case Region::pontic_steppe: return 52;
            case Region::anatolia: return 72;
            case Region::south_germany: return 24;
            case Region::danubia: return 20;
            case Region::poland: return 18;
            default: return 35;
        }
    }

    int sameRegionNeighborCount(const MapState& state, int width, int height, int x, int y, Region region)
    {
        int count = 0;

        for (const auto& d : kDirs)
        {
            const int nx = x + d[0];
            const int ny = y + d[1];

            if (!inBoundsRaw(nx, ny, width, height))
            {
                continue;
            }

            if (state.owner[static_cast<size_t>(indexRaw(nx, ny, width))] == region)
            {
                ++count;
            }
        }

        return count;
    }

    int edgeDistance(int x, int y, int width, int height)
    {
        const int left = x;
        const int right = width - 1 - x;
        const int top = y;
        const int bottom = height - 1 - y;
        return std::min(std::min(left, right), std::min(top, bottom));
    }

    bool shouldFloodFromEdge(
        const MapState& state,
        int width,
        int height,
        int x,
        int y,
        std::uint32_t seed,
        int baseDepth)
    {
        const Region region = state.owner[static_cast<size_t>(indexRaw(x, y, width))];
        const int coastBias = coastalNeed(region);
        const int edge = edgeDistance(x, y, width, height);
        const int noise = tileNoise(x, y, seed);

        int threshold = baseDepth;
        threshold += coastBias / 12;
        threshold += noise / 18;

        if (region == Region::british_isles)
        {
            threshold += 3;
        }

        return edge <= threshold;
    }

    void floodOceanFromEdges(std::vector<bool>& mask, const MapState& state, int width, int height, std::uint32_t seed)
    {
        std::vector<int> visited(static_cast<size_t>(width * height), 0);
        std::queue<OceanNode> queue;
        const int baseDepth = std::max(2, std::min(width, height) / 10);

        for (int x = 0; x < width; ++x)
        {
            for (int y : {0, height - 1})
            {
                if (shouldFloodFromEdge(state, width, height, x, y, seed, baseDepth))
                {
                    const int idx = indexRaw(x, y, width);
                    if (visited[static_cast<size_t>(idx)] == 0)
                    {
                        visited[static_cast<size_t>(idx)] = 1;
                        queue.push(OceanNode{x, y});
                    }
                }
            }
        }

        for (int y = 0; y < height; ++y)
        {
            for (int x : {0, width - 1})
            {
                if (shouldFloodFromEdge(state, width, height, x, y, seed, baseDepth))
                {
                    const int idx = indexRaw(x, y, width);
                    if (visited[static_cast<size_t>(idx)] == 0)
                    {
                        visited[static_cast<size_t>(idx)] = 1;
                        queue.push(OceanNode{x, y});
                    }
                }
            }
        }

        while (!queue.empty())
        {
            const OceanNode current = queue.front();
            queue.pop();

            const int idx = indexRaw(current.x, current.y, width);
            const Region region = state.owner[static_cast<size_t>(idx)];

            if (sameRegionNeighborCount(state, width, height, current.x, current.y, region) <= 1 && edgeDistance(current.x, current.y, width, height) > 0)
            {
                continue;
            }

            mask[static_cast<size_t>(idx)] = false;

            for (const auto& d : kDirs)
            {
                const int nx = current.x + d[0];
                const int ny = current.y + d[1];

                if (!inBoundsRaw(nx, ny, width, height))
                {
                    continue;
                }

                const int ni = indexRaw(nx, ny, width);
                if (visited[static_cast<size_t>(ni)] != 0)
                {
                    continue;
                }

                if (!shouldFloodFromEdge(state, width, height, nx, ny, seed + 17u, baseDepth))
                {
                    continue;
                }

                visited[static_cast<size_t>(ni)] = 1;
                queue.push(OceanNode{nx, ny});
            }
        }
    }

    void enforceIllegalBorderWater(std::vector<bool>& mask, const MapState& state, int width, int height)
    {
        std::vector<bool> next = mask;

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const int idx = indexRaw(x, y, width);
                if (!mask[static_cast<size_t>(idx)])
                {
                    continue;
                }

                const Region region = state.owner[static_cast<size_t>(idx)];

                for (const auto& d : kDirs)
                {
                    const int nx = x + d[0];
                    const int ny = y + d[1];

                    if (!inBoundsRaw(nx, ny, width, height))
                    {
                        continue;
                    }

                    const int ni = indexRaw(nx, ny, width);
                    const Region neighbor = state.owner[static_cast<size_t>(ni)];

                    if (neighbor == region || neighbor == Region::none)
                    {
                        continue;
                    }

                    if (!regionsAreAdjacentInRules(region, neighbor))
                    {
                        next[static_cast<size_t>(idx)] = false;
                        next[static_cast<size_t>(ni)] = false;
                    }
                }
            }
        }

        mask.swap(next);
    }

    void enforceBritishWater(std::vector<bool>& mask, const MapState& state, int width, int height)
    {
        std::vector<bool> next = mask;

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const int idx = indexRaw(x, y, width);
                const Region region = state.owner[static_cast<size_t>(idx)];

                if (region != Region::british_isles || !mask[static_cast<size_t>(idx)])
                {
                    continue;
                }

                for (const auto& d : kDirs)
                {
                    const int nx = x + d[0];
                    const int ny = y + d[1];

                    if (!inBoundsRaw(nx, ny, width, height))
                    {
                        continue;
                    }

                    const int ni = indexRaw(nx, ny, width);
                    const Region neighbor = state.owner[static_cast<size_t>(ni)];

                    if (neighbor != Region::none && neighbor != Region::british_isles)
                    {
                        next[static_cast<size_t>(ni)] = false;
                    }
                }
            }
        }

        mask.swap(next);
    }

    void softenCoastline(std::vector<bool>& mask, int width, int height, int passes)
    {
        for (int pass = 0; pass < passes; ++pass)
        {
            std::vector<bool> next = mask;

            for (int y = 1; y < height - 1; ++y)
            {
                for (int x = 1; x < width - 1; ++x)
                {
                    const int idx = indexRaw(x, y, width);
                    int landNeighbors = 0;

                    for (const auto& d : kDiagDirs)
                    {
                        const int nx = x + d[0];
                        const int ny = y + d[1];
                        if (mask[static_cast<size_t>(indexRaw(nx, ny, width))])
                        {
                            ++landNeighbors;
                        }
                    }

                    if (mask[static_cast<size_t>(idx)] && landNeighbors <= 2)
                    {
                        next[static_cast<size_t>(idx)] = false;
                    }
                    else if (!mask[static_cast<size_t>(idx)] && landNeighbors >= 7)
                    {
                        next[static_cast<size_t>(idx)] = true;
                    }
                }
            }

            mask.swap(next);
        }
    }

    void keepLargestGlobalLandmassPlusBritain(std::vector<bool>& mask, const MapState& state, int width, int height)
    {
        std::vector<int> visited(static_cast<size_t>(width * height), 0);
        std::vector<int> bestComponent;
        std::queue<int> queue;

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const int start = indexRaw(x, y, width);
                if (!mask[static_cast<size_t>(start)] || visited[static_cast<size_t>(start)] != 0)
                {
                    continue;
                }

                const Region region = state.owner[static_cast<size_t>(start)];
                if (region == Region::british_isles)
                {
                    continue;
                }

                std::vector<int> component;
                visited[static_cast<size_t>(start)] = 1;
                queue.push(start);

                while (!queue.empty())
                {
                    const int idx = queue.front();
                    queue.pop();
                    component.push_back(idx);

                    const int cx = idx % width;
                    const int cy = idx / width;

                    for (const auto& d : kDirs)
                    {
                        const int nx = cx + d[0];
                        const int ny = cy + d[1];
                        if (!inBoundsRaw(nx, ny, width, height))
                        {
                            continue;
                        }

                        const int ni = indexRaw(nx, ny, width);
                        if (!mask[static_cast<size_t>(ni)] || visited[static_cast<size_t>(ni)] != 0)
                        {
                            continue;
                        }

                        if (state.owner[static_cast<size_t>(ni)] == Region::british_isles)
                        {
                            continue;
                        }

                        visited[static_cast<size_t>(ni)] = 1;
                        queue.push(ni);
                    }
                }

                if (component.size() > bestComponent.size())
                {
                    bestComponent.swap(component);
                }
            }
        }

        std::vector<bool> keep(static_cast<size_t>(width * height), false);

        for (int idx : bestComponent)
        {
            keep[static_cast<size_t>(idx)] = true;
        }

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const int idx = indexRaw(x, y, width);
                if (mask[static_cast<size_t>(idx)] && state.owner[static_cast<size_t>(idx)] == Region::british_isles)
                {
                    keep[static_cast<size_t>(idx)] = true;
                }
            }
        }

        mask.swap(keep);
    }
}

LandProfile buildLandProfile(const Map& map, std::mt19937& rng)
{
    LandProfile profile;
    profile.westCoastBase = std::max(2, map.width() / 16 + randInt(rng, -1, 2));
    profile.northCoastBase = std::max(2, map.height() / 14 + randInt(rng, -1, 2));
    profile.southCoastBase = std::max(2, map.height() / 14 + randInt(rng, -1, 2));
    profile.eastOpenBias = std::max(2, map.width() / 14 + randInt(rng, -1, 2));
    return profile;
}

BritainShape buildBritainShape(const Map& map, const MapState& state, std::mt19937& rng)
{
    BritainShape britain;
    britain.centerX = std::max(4, map.width() / 7);
    britain.centerY = std::max(4, map.height() / 4);
    britain.radiusX = std::max(3, map.width() / 20);
    britain.radiusY = std::max(4, map.height() / 14);
    britain.channelX = britain.centerX + britain.radiusX + randInt(rng, 1, 3);
    britain.channelWidth = 1;

    for (const auto& seed : state.seeds)
    {
        if (seed.region == Region::british_isles)
        {
            britain.centerX = seed.x;
            britain.centerY = seed.y;
            break;
        }
    }

    return britain;
}

bool isInsideBritainShape(int x, int y, const BritainShape& britain)
{
    const double dx = static_cast<double>(x - britain.centerX) / std::max(1, britain.radiusX);
    const double dy = static_cast<double>(y - britain.centerY) / std::max(1, britain.radiusY);
    return (dx * dx) + (dy * dy) <= 1.0;
}

bool isInsideMainlandEnvelope(int x, int y, int width, int height, const LandProfile& profile)
{
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    (void)profile;
    return true;
}

void carveCoastlineNoise(std::vector<bool>& mask, int width, int height, std::mt19937& rng)
{
    softenCoastline(mask, width, height, std::max(1, randInt(rng, 1, 2)));
}

void enforceBritishChannel(std::vector<bool>& mask, int width, int height, const BritainShape& britain)
{
    (void)mask;
    (void)width;
    (void)height;
    (void)britain;
}

void keepLargestMainland(std::vector<bool>& mask, int width, int height, const BritainShape& britain)
{
    (void)britain;
    MapState dummyState;
    dummyState.owner.resize(static_cast<size_t>(width * height), Region::none);
    keepLargestGlobalLandmassPlusBritain(mask, dummyState, width, height);
}

void applyLandmask(Map& map, MapState& state)
{
    const int width = map.width();
    const int height = map.height();

    state.landMask.assign(static_cast<size_t>(width * height), true);

    floodOceanFromEdges(state.landMask, state, width, height, map.seed() + 97u);
    enforceIllegalBorderWater(state.landMask, state, width, height);
    enforceBritishWater(state.landMask, state, width, height);
    softenCoastline(state.landMask, width, height, 1);
    keepLargestGlobalLandmassPlusBritain(state.landMask, state, width, height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const int idx = indexRaw(x, y, width);
            Tile& tile = map.tile(x, y);

            tile.land = state.landMask[static_cast<size_t>(idx)];
            tile.region = tile.land ? state.owner[static_cast<size_t>(idx)] : Region::none;
            tile.terrain = tile.land ? Terrain::plains : Terrain::ocean;
        }
    }
}