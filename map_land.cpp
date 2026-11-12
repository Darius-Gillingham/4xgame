// File: map_land.cpp
// Commit: Apply horizontal wraparound to land carving and coastline adjacency

#include "map_internal.h"
#include "map_region_plan.h"

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

    int wrapRawX(int x, int width)
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

    bool validRawY(int y, int height)
    {
        return y >= 0 && y < height;
    }

    int indexRaw(int x, int y, int width)
    {
        return y * width + wrapRawX(x, width);
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
        value ^= static_cast<std::uint32_t>(wrapRawX(x, 1000003) * 73856093);
        value ^= static_cast<std::uint32_t>(y * 19349663);
        value = hash32(value);
        return static_cast<int>(value % 101U) - 50;
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

            case Region::levant: return 72;
            case Region::mesopotamia: return 36;
            case Region::arabia: return 82;
            case Region::caucasus: return 38;
            case Region::persia: return 36;
            case Region::transoxiana: return 12;
            case Region::kazakhstan_steppe: return 10;
            case Region::tarim_basin: return 8;
            case Region::mongolia: return 10;
            case Region::northwest_india: return 42;
            case Region::gangetic_plain: return 28;
            case Region::deccan: return 78;
            case Region::bengal: return 70;
            case Region::sri_lanka: return 96;
            case Region::north_china: return 42;
            case Region::central_china: return 24;
            case Region::south_china: return 78;
            case Region::manchuria: return 58;
            case Region::korea: return 88;
            case Region::japan: return 96;
            case Region::indochina: return 86;
            case Region::mainland_southeast_asia: return 62;
            case Region::malay_peninsula: return 94;
            case Region::indonesia: return 98;
            case Region::philippines: return 98;

            case Region::egypt: return 62;
            case Region::libya: return 38;
            case Region::maghreb: return 72;
            case Region::nubia: return 20;
            case Region::horn_of_africa: return 86;
            case Region::ethiopian_highlands: return 26;
            case Region::swahili_coast: return 88;
            case Region::sahel: return 12;
            case Region::upper_niger: return 20;
            case Region::guinea_coast: return 88;
            case Region::congo_basin: return 32;
            case Region::southern_africa: return 82;

            case Region::arctic_america: return 92;
            case Region::western_canada: return 48;
            case Region::eastern_canada: return 72;
            case Region::pacific_northwest: return 88;
            case Region::great_plains: return 14;
            case Region::great_lakes: return 36;
            case Region::northeast_america: return 82;
            case Region::california: return 82;
            case Region::southwest_america: return 30;
            case Region::southeast_america: return 84;
            case Region::mexico: return 82;
            case Region::central_america: return 94;
            case Region::caribbean: return 98;
            case Region::andes: return 86;
            case Region::amazon: return 42;
            case Region::brazil: return 88;
            case Region::southern_cone: return 72;
            case Region::patagonia: return 88;

            case Region::australia: return 92;
            case Region::new_guinea: return 96;
            case Region::new_zealand: return 96;
            case Region::pacific_islands: return 98;

            default: return 35;
        }
    }

    bool isIslandRegion(Region region)
    {
        return
            region == Region::british_isles ||
            region == Region::sri_lanka ||
            region == Region::japan ||
            region == Region::indonesia ||
            region == Region::philippines ||
            region == Region::caribbean ||
            region == Region::new_guinea ||
            region == Region::new_zealand ||
            region == Region::pacific_islands;
    }

    int sameRegionNeighborCount(const MapState& state, int width, int height, int x, int y, Region region)
    {
        int count = 0;

        for (const auto& d : kDirs)
        {
            const int nx = wrapRawX(x + d[0], width);
            const int ny = y + d[1];

            if (!validRawY(ny, height))
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

    int edgeDistance(int y, int height)
    {
        const int top = y;
        const int bottom = height - 1 - y;
        return std::min(top, bottom);
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
        const int edge = edgeDistance(y, height);
        const int noise = tileNoise(x, y, seed);

        int threshold = baseDepth;
        threshold += coastBias / 12;
        threshold += noise / 18;

        if (isIslandRegion(region))
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

        while (!queue.empty())
        {
            const OceanNode current = queue.front();
            queue.pop();

            const int idx = indexRaw(current.x, current.y, width);
            const Region region = state.owner[static_cast<size_t>(idx)];

            if (sameRegionNeighborCount(state, width, height, current.x, current.y, region) <= 1 &&
                edgeDistance(current.y, height) > 0)
            {
                continue;
            }

            mask[static_cast<size_t>(idx)] = false;

            for (const auto& d : kDirs)
            {
                const int nx = wrapRawX(current.x + d[0], width);
                const int ny = current.y + d[1];

                if (!validRawY(ny, height))
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
                    const int nx = wrapRawX(x + d[0], width);
                    const int ny = y + d[1];

                    if (!validRawY(ny, height))
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

    void enforceIslandWater(std::vector<bool>& mask, const MapState& state, int width, int height)
    {
        std::vector<bool> next = mask;

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const int idx = indexRaw(x, y, width);
                const Region region = state.owner[static_cast<size_t>(idx)];

                if (!isIslandRegion(region) || !mask[static_cast<size_t>(idx)])
                {
                    continue;
                }

                for (const auto& d : kDirs)
                {
                    const int nx = wrapRawX(x + d[0], width);
                    const int ny = y + d[1];

                    if (!validRawY(ny, height))
                    {
                        continue;
                    }

                    const int ni = indexRaw(nx, ny, width);
                    const Region neighbor = state.owner[static_cast<size_t>(ni)];

                    if (neighbor != Region::none && neighbor != region)
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
                for (int x = 0; x < width; ++x)
                {
                    const int idx = indexRaw(x, y, width);
                    int landNeighbors = 0;

                    for (const auto& d : kDiagDirs)
                    {
                        const int nx = wrapRawX(x + d[0], width);
                        const int ny = y + d[1];

                        if (!validRawY(ny, height))
                        {
                            continue;
                        }

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

    void keepLandmasses(std::vector<bool>& mask, const MapState& state, int width, int height)
    {
        std::vector<int> visited(static_cast<size_t>(width * height), 0);
        std::vector<bool> keep(static_cast<size_t>(width * height), false);
        std::queue<int> queue;

        for (const RegionPlan& plan : allRegionPlans())
        {
            std::vector<int> bestComponent;

            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    const int start = indexRaw(x, y, width);

                    if (!mask[static_cast<size_t>(start)] || visited[static_cast<size_t>(start)] != 0)
                    {
                        continue;
                    }

                    if (state.owner[static_cast<size_t>(start)] != plan.region)
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
                            const int nx = wrapRawX(cx + d[0], width);
                            const int ny = cy + d[1];

                            if (!validRawY(ny, height))
                            {
                                continue;
                            }

                            const int ni = indexRaw(nx, ny, width);

                            if (!mask[static_cast<size_t>(ni)] || visited[static_cast<size_t>(ni)] != 0)
                            {
                                continue;
                            }

                            if (state.owner[static_cast<size_t>(ni)] != plan.region)
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

            for (int idx : bestComponent)
            {
                keep[static_cast<size_t>(idx)] = true;
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

    for (const RegionSeed& seed : state.seeds)
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

    if (allRegionPlans().empty())
    {
        return;
    }

    keepLandmasses(mask, dummyState, width, height);
}

void applyLandmask(Map& map, MapState& state)
{
    const int width = map.width();
    const int height = map.height();

    state.landMask.assign(static_cast<size_t>(width * height), true);

    floodOceanFromEdges(state.landMask, state, width, height, map.seed() + 97u);
    enforceIllegalBorderWater(state.landMask, state, width, height);
    enforceIslandWater(state.landMask, state, width, height);
    softenCoastline(state.landMask, width, height, 1);
    keepLandmasses(state.landMask, state, width, height);

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