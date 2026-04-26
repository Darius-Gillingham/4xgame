// File: map_terrain.cpp
// Commit: Remove A suffixes from terrain assignment and smoothing pipeline

#include "map_internal.h"
#include <queue>
#include <vector>

Terrain pickTerrainForRegion(Region region, std::mt19937& rng)
{
    switch (region)
    {
        case Region::scandinavia:
            return pickFrom(std::vector<Terrain>{Terrain::tundra, Terrain::boreal, Terrain::boreal, Terrain::woods, Terrain::hills, Terrain::mountains}, rng);
        case Region::british_isles:
            return pickFrom(std::vector<Terrain>{Terrain::plains, Terrain::farmlands, Terrain::woods, Terrain::marsh, Terrain::hills}, rng);
        case Region::baltic:
            return pickFrom(std::vector<Terrain>{Terrain::boreal, Terrain::woods, Terrain::plains, Terrain::marsh}, rng);
        case Region::russia:
            return pickFrom(std::vector<Terrain>{Terrain::boreal, Terrain::woods, Terrain::plains, Terrain::marsh}, rng);
        case Region::france:
            return pickFrom(std::vector<Terrain>{Terrain::plains, Terrain::farmlands, Terrain::farmlands, Terrain::woods, Terrain::hills}, rng);
        case Region::lowlands:
            return pickFrom(std::vector<Terrain>{Terrain::farmlands, Terrain::farmlands, Terrain::plains, Terrain::marsh}, rng);
        case Region::north_germany:
            return pickFrom(std::vector<Terrain>{Terrain::plains, Terrain::farmlands, Terrain::woods, Terrain::marsh}, rng);
        case Region::south_germany:
            return pickFrom(std::vector<Terrain>{Terrain::woods, Terrain::plains, Terrain::farmlands, Terrain::hills, Terrain::highlands}, rng);
        case Region::poland:
            return pickFrom(std::vector<Terrain>{Terrain::plains, Terrain::farmlands, Terrain::woods, Terrain::marsh}, rng);
        case Region::iberia:
            return pickFrom(std::vector<Terrain>{Terrain::plains, Terrain::farmlands, Terrain::hills, Terrain::arid, Terrain::highlands}, rng);
        case Region::italy:
            return pickFrom(std::vector<Terrain>{Terrain::farmlands, Terrain::plains, Terrain::hills, Terrain::highlands, Terrain::mountains}, rng);
        case Region::danubia:
            return pickFrom(std::vector<Terrain>{Terrain::farmlands, Terrain::farmlands, Terrain::plains, Terrain::plains, Terrain::hills}, rng);
        case Region::balkans:
            return pickFrom(std::vector<Terrain>{Terrain::hills, Terrain::highlands, Terrain::mountains, Terrain::plains}, rng);
        case Region::pontic_steppe:
            return pickFrom(std::vector<Terrain>{Terrain::steppe, Terrain::steppe, Terrain::plains, Terrain::arid, Terrain::hills}, rng);
        case Region::anatolia:
            return pickFrom(std::vector<Terrain>{Terrain::plains, Terrain::hills, Terrain::highlands, Terrain::arid, Terrain::desert}, rng);
        default:
            return Terrain::plains;
    }
}

void assignTerrainClusters(Map& map, std::uint32_t seed)
{
    std::mt19937 rng(seed + 1u);
    std::vector<Terrain> next(static_cast<size_t>(map.width() * map.height()), Terrain::ocean);

    for (int y = 0; y < map.height(); ++y) for (int x = 0; x < map.width(); ++x)
    {
        int start = indexInternal(map, x, y);
        if (!map.at(x, y).land || next[static_cast<size_t>(start)] != Terrain::ocean) continue;

        Terrain terrain = pickTerrainForRegion(map.at(x, y).region, rng);
        Region region = map.at(x, y).region;
        std::queue<int> q; q.push(start); next[static_cast<size_t>(start)] = terrain;
        int placed = 1, target = randInt(rng, 6, 28);

        while (!q.empty() && placed < target)
        {
            int idx = q.front(); q.pop();
            int cx = idx % map.width(), cy = idx / map.width();

            for (const auto& d : kDirs)
            {
                int nx = cx + d[0], ny = cy + d[1], ni = indexInternal(map, nx, ny);
                if (!inBoundsInternal(map, nx, ny) || !map.at(nx, ny).land) continue;
                if (map.at(nx, ny).region != region || next[static_cast<size_t>(ni)] != Terrain::ocean) continue;
                if (randInt(rng, 0, 99) >= 90) continue;
                next[static_cast<size_t>(ni)] = terrain;
                q.push(ni);
                if (++placed >= target) break;
            }
        }
    }

    for (int y = 0; y < map.height(); ++y) for (int x = 0; x < map.width(); ++x)
    {
        if (!map.tile(x, y).land) continue;
        int idx = indexInternal(map, x, y);
        if (next[static_cast<size_t>(idx)] == Terrain::ocean)
            next[static_cast<size_t>(idx)] = pickTerrainForRegion(map.at(x, y).region, rng);
        map.tile(x, y).terrain = next[static_cast<size_t>(idx)];
    }
}

void smoothTerrainPasses(Map& map)
{
    for (int pass = 0; pass < 3; ++pass)
    {
        std::vector<Terrain> next(static_cast<size_t>(map.width() * map.height()), Terrain::ocean);

        for (int y = 0; y < map.height(); ++y) for (int x = 0; x < map.width(); ++x)
        {
            if (!map.at(x, y).land) continue;
            int counts[13] = {0};
            counts[static_cast<int>(map.at(x, y).terrain)] += 4;

            for (const auto& d : kDirs)
            {
                int nx = x + d[0], ny = y + d[1];
                if (inBoundsInternal(map, nx, ny) && map.at(nx, ny).land)
                    counts[static_cast<int>(map.at(nx, ny).terrain)] += 3;
            }

            int best = static_cast<int>(map.at(x, y).terrain);
            for (int i = 0; i < 13; ++i) if (counts[i] > counts[best]) best = i;
            next[static_cast<size_t>(indexInternal(map, x, y))] = static_cast<Terrain>(best);
        }

        for (int y = 0; y < map.height(); ++y) for (int x = 0; x < map.width(); ++x)
            if (map.tile(x, y).land)
                map.tile(x, y).terrain = next[static_cast<size_t>(indexInternal(map, x, y))];
    }
}