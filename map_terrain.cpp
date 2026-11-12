// File: map_terrain.cpp
// Commit: Add Americas and Oceania terrain profiles for expanded world generation

#include "map_internal.h"

#include <queue>
#include <vector>

namespace
{
    constexpr int kTerrainCount = static_cast<int>(Terrain::ocean) + 1;
}

Terrain pickTerrainForRegion(Region region, std::mt19937& rng)
{
    switch (region)
    {
        case Region::scandinavia:
            return pickFrom(std::vector<Terrain>{Terrain::tundra, Terrain::boreal, Terrain::boreal, Terrain::woods, Terrain::hills, Terrain::mountains}, rng);
        case Region::british_isles:
            return pickFrom(std::vector<Terrain>{Terrain::meadow, Terrain::meadow, Terrain::woods, Terrain::marsh, Terrain::hills, Terrain::highlands}, rng);
        case Region::baltic:
            return pickFrom(std::vector<Terrain>{Terrain::boreal, Terrain::woods, Terrain::woods, Terrain::meadow, Terrain::marsh}, rng);
        case Region::russia:
            return pickFrom(std::vector<Terrain>{Terrain::boreal, Terrain::boreal, Terrain::woods, Terrain::meadow, Terrain::marsh, Terrain::steppe}, rng);
        case Region::france:
            return pickFrom(std::vector<Terrain>{Terrain::meadow, Terrain::meadow, Terrain::woods, Terrain::woods, Terrain::hills, Terrain::marsh}, rng);
        case Region::lowlands:
            return pickFrom(std::vector<Terrain>{Terrain::meadow, Terrain::meadow, Terrain::marsh, Terrain::marsh, Terrain::woods}, rng);
        case Region::north_germany:
            return pickFrom(std::vector<Terrain>{Terrain::meadow, Terrain::woods, Terrain::woods, Terrain::marsh, Terrain::hills}, rng);
        case Region::south_germany:
            return pickFrom(std::vector<Terrain>{Terrain::woods, Terrain::woods, Terrain::meadow, Terrain::hills, Terrain::highlands, Terrain::mountains}, rng);
        case Region::poland:
            return pickFrom(std::vector<Terrain>{Terrain::meadow, Terrain::woods, Terrain::woods, Terrain::marsh, Terrain::hills}, rng);
        case Region::iberia:
            return pickFrom(std::vector<Terrain>{Terrain::meadow, Terrain::hills, Terrain::arid, Terrain::arid, Terrain::highlands, Terrain::mountains}, rng);
        case Region::italy:
            return pickFrom(std::vector<Terrain>{Terrain::meadow, Terrain::meadow, Terrain::hills, Terrain::highlands, Terrain::mountains, Terrain::arid}, rng);
        case Region::danubia:
            return pickFrom(std::vector<Terrain>{Terrain::meadow, Terrain::meadow, Terrain::floodplain, Terrain::woods, Terrain::hills}, rng);
        case Region::balkans:
            return pickFrom(std::vector<Terrain>{Terrain::hills, Terrain::highlands, Terrain::mountains, Terrain::woods, Terrain::meadow}, rng);
        case Region::pontic_steppe:
            return pickFrom(std::vector<Terrain>{Terrain::steppe, Terrain::steppe, Terrain::steppe, Terrain::arid, Terrain::hills, Terrain::meadow}, rng);
        case Region::anatolia:
            return pickFrom(std::vector<Terrain>{Terrain::hills, Terrain::highlands, Terrain::arid, Terrain::arid, Terrain::desert, Terrain::oasis, Terrain::mountains}, rng);

        case Region::levant:
            return pickFrom(std::vector<Terrain>{Terrain::arid, Terrain::arid, Terrain::hills, Terrain::meadow, Terrain::floodplain, Terrain::oasis}, rng);
        case Region::mesopotamia:
            return pickFrom(std::vector<Terrain>{Terrain::floodplain, Terrain::floodplain, Terrain::arid, Terrain::marsh, Terrain::desert, Terrain::oasis}, rng);
        case Region::arabia:
            return pickFrom(std::vector<Terrain>{Terrain::desert, Terrain::desert, Terrain::desert, Terrain::arid, Terrain::oasis, Terrain::hills}, rng);
        case Region::caucasus:
            return pickFrom(std::vector<Terrain>{Terrain::mountains, Terrain::highlands, Terrain::hills, Terrain::woods, Terrain::meadow}, rng);
        case Region::persia:
            return pickFrom(std::vector<Terrain>{Terrain::highlands, Terrain::mountains, Terrain::arid, Terrain::desert, Terrain::oasis, Terrain::steppe}, rng);
        case Region::transoxiana:
            return pickFrom(std::vector<Terrain>{Terrain::steppe, Terrain::arid, Terrain::floodplain, Terrain::oasis, Terrain::desert}, rng);
        case Region::kazakhstan_steppe:
            return pickFrom(std::vector<Terrain>{Terrain::steppe, Terrain::steppe, Terrain::steppe, Terrain::arid, Terrain::meadow}, rng);
        case Region::tarim_basin:
            return pickFrom(std::vector<Terrain>{Terrain::desert, Terrain::desert, Terrain::arid, Terrain::oasis, Terrain::mountains}, rng);
        case Region::mongolia:
            return pickFrom(std::vector<Terrain>{Terrain::steppe, Terrain::steppe, Terrain::arid, Terrain::hills, Terrain::mountains, Terrain::desert}, rng);
        case Region::northwest_india:
            return pickFrom(std::vector<Terrain>{Terrain::arid, Terrain::meadow, Terrain::floodplain, Terrain::hills, Terrain::desert}, rng);
        case Region::gangetic_plain:
            return pickFrom(std::vector<Terrain>{Terrain::floodplain, Terrain::floodplain, Terrain::meadow, Terrain::marsh, Terrain::woods}, rng);
        case Region::deccan:
            return pickFrom(std::vector<Terrain>{Terrain::savanna, Terrain::savanna, Terrain::hills, Terrain::highlands, Terrain::woods, Terrain::arid}, rng);
        case Region::bengal:
            return pickFrom(std::vector<Terrain>{Terrain::floodplain, Terrain::marsh, Terrain::jungle, Terrain::jungle, Terrain::meadow}, rng);
        case Region::sri_lanka:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::jungle_highlands, Terrain::meadow, Terrain::hills}, rng);
        case Region::north_china:
            return pickFrom(std::vector<Terrain>{Terrain::meadow, Terrain::steppe, Terrain::floodplain, Terrain::hills, Terrain::arid}, rng);
        case Region::central_china:
            return pickFrom(std::vector<Terrain>{Terrain::floodplain, Terrain::meadow, Terrain::woods, Terrain::hills, Terrain::highlands}, rng);
        case Region::south_china:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::woods, Terrain::floodplain, Terrain::hills, Terrain::jungle_highlands}, rng);
        case Region::manchuria:
            return pickFrom(std::vector<Terrain>{Terrain::boreal, Terrain::woods, Terrain::meadow, Terrain::hills, Terrain::mountains}, rng);
        case Region::korea:
            return pickFrom(std::vector<Terrain>{Terrain::woods, Terrain::hills, Terrain::mountains, Terrain::meadow}, rng);
        case Region::japan:
            return pickFrom(std::vector<Terrain>{Terrain::woods, Terrain::hills, Terrain::mountains, Terrain::meadow, Terrain::jungle_highlands}, rng);
        case Region::indochina:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::jungle, Terrain::hills, Terrain::floodplain, Terrain::jungle_highlands}, rng);
        case Region::mainland_southeast_asia:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::floodplain, Terrain::woods, Terrain::hills, Terrain::jungle_highlands}, rng);
        case Region::malay_peninsula:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::jungle, Terrain::marsh, Terrain::hills}, rng);
        case Region::indonesia:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::jungle, Terrain::jungle_highlands, Terrain::mountains, Terrain::marsh}, rng);
        case Region::philippines:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::jungle_highlands, Terrain::hills, Terrain::mountains, Terrain::marsh}, rng);

        case Region::egypt:
            return pickFrom(std::vector<Terrain>{Terrain::floodplain, Terrain::floodplain, Terrain::desert, Terrain::arid, Terrain::oasis}, rng);
        case Region::libya:
            return pickFrom(std::vector<Terrain>{Terrain::desert, Terrain::desert, Terrain::desert, Terrain::arid, Terrain::oasis}, rng);
        case Region::maghreb:
            return pickFrom(std::vector<Terrain>{Terrain::arid, Terrain::hills, Terrain::desert, Terrain::meadow, Terrain::mountains}, rng);
        case Region::nubia:
            return pickFrom(std::vector<Terrain>{Terrain::desert, Terrain::arid, Terrain::floodplain, Terrain::hills, Terrain::oasis}, rng);
        case Region::horn_of_africa:
            return pickFrom(std::vector<Terrain>{Terrain::arid, Terrain::hills, Terrain::highlands, Terrain::savanna, Terrain::desert}, rng);
        case Region::ethiopian_highlands:
            return pickFrom(std::vector<Terrain>{Terrain::highlands, Terrain::mountains, Terrain::hills, Terrain::meadow, Terrain::savanna}, rng);
        case Region::swahili_coast:
            return pickFrom(std::vector<Terrain>{Terrain::savanna, Terrain::jungle, Terrain::jungle, Terrain::hills, Terrain::meadow}, rng);
        case Region::sahel:
            return pickFrom(std::vector<Terrain>{Terrain::savanna, Terrain::savanna, Terrain::arid, Terrain::steppe, Terrain::desert}, rng);
        case Region::upper_niger:
            return pickFrom(std::vector<Terrain>{Terrain::savanna, Terrain::floodplain, Terrain::woods, Terrain::hills, Terrain::arid}, rng);
        case Region::guinea_coast:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::jungle, Terrain::marsh, Terrain::savanna, Terrain::floodplain}, rng);
        case Region::congo_basin:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::jungle, Terrain::jungle, Terrain::marsh, Terrain::floodplain}, rng);
        case Region::southern_africa:
            return pickFrom(std::vector<Terrain>{Terrain::savanna, Terrain::savanna, Terrain::arid, Terrain::hills, Terrain::highlands, Terrain::desert}, rng);

        case Region::arctic_america:
            return pickFrom(std::vector<Terrain>{Terrain::tundra, Terrain::tundra, Terrain::boreal, Terrain::marsh, Terrain::mountains}, rng);
        case Region::western_canada:
            return pickFrom(std::vector<Terrain>{Terrain::boreal, Terrain::boreal, Terrain::woods, Terrain::mountains, Terrain::highlands, Terrain::marsh}, rng);
        case Region::eastern_canada:
            return pickFrom(std::vector<Terrain>{Terrain::boreal, Terrain::woods, Terrain::woods, Terrain::marsh, Terrain::meadow, Terrain::hills}, rng);
        case Region::pacific_northwest:
            return pickFrom(std::vector<Terrain>{Terrain::woods, Terrain::woods, Terrain::mountains, Terrain::highlands, Terrain::marsh}, rng);
        case Region::great_plains:
            return pickFrom(std::vector<Terrain>{Terrain::plains, Terrain::plains, Terrain::steppe, Terrain::meadow, Terrain::hills}, rng);
        case Region::great_lakes:
            return pickFrom(std::vector<Terrain>{Terrain::woods, Terrain::meadow, Terrain::marsh, Terrain::plains, Terrain::hills}, rng);
        case Region::northeast_america:
            return pickFrom(std::vector<Terrain>{Terrain::woods, Terrain::woods, Terrain::meadow, Terrain::hills, Terrain::marsh}, rng);
        case Region::california:
            return pickFrom(std::vector<Terrain>{Terrain::hills, Terrain::arid, Terrain::meadow, Terrain::woods, Terrain::mountains}, rng);
        case Region::southwest_america:
            return pickFrom(std::vector<Terrain>{Terrain::desert, Terrain::arid, Terrain::hills, Terrain::mountains, Terrain::oasis}, rng);
        case Region::southeast_america:
            return pickFrom(std::vector<Terrain>{Terrain::woods, Terrain::marsh, Terrain::floodplain, Terrain::meadow, Terrain::jungle}, rng);
        case Region::mexico:
            return pickFrom(std::vector<Terrain>{Terrain::hills, Terrain::highlands, Terrain::arid, Terrain::desert, Terrain::jungle, Terrain::meadow}, rng);
        case Region::central_america:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::jungle, Terrain::jungle_highlands, Terrain::hills, Terrain::marsh}, rng);
        case Region::caribbean:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::jungle, Terrain::meadow, Terrain::hills, Terrain::marsh}, rng);
        case Region::andes:
            return pickFrom(std::vector<Terrain>{Terrain::mountains, Terrain::mountains, Terrain::highlands, Terrain::arid, Terrain::jungle_highlands}, rng);
        case Region::amazon:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::jungle, Terrain::jungle, Terrain::marsh, Terrain::floodplain}, rng);
        case Region::brazil:
            return pickFrom(std::vector<Terrain>{Terrain::savanna, Terrain::jungle, Terrain::woods, Terrain::hills, Terrain::meadow}, rng);
        case Region::southern_cone:
            return pickFrom(std::vector<Terrain>{Terrain::plains, Terrain::steppe, Terrain::meadow, Terrain::hills, Terrain::arid}, rng);
        case Region::patagonia:
            return pickFrom(std::vector<Terrain>{Terrain::steppe, Terrain::arid, Terrain::hills, Terrain::mountains, Terrain::tundra}, rng);

        case Region::australia:
            return pickFrom(std::vector<Terrain>{Terrain::desert, Terrain::desert, Terrain::arid, Terrain::savanna, Terrain::hills, Terrain::meadow}, rng);
        case Region::new_guinea:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::jungle, Terrain::jungle_highlands, Terrain::mountains, Terrain::marsh}, rng);
        case Region::new_zealand:
            return pickFrom(std::vector<Terrain>{Terrain::meadow, Terrain::woods, Terrain::highlands, Terrain::mountains, Terrain::marsh}, rng);
        case Region::pacific_islands:
            return pickFrom(std::vector<Terrain>{Terrain::jungle, Terrain::jungle, Terrain::meadow, Terrain::hills, Terrain::marsh}, rng);

        default:
            return Terrain::plains;
    }
}

void assignTerrainClusters(Map& map, std::uint32_t seed)
{
    std::mt19937 rng(seed + 2027u);

    for (int y = 0; y < map.height(); ++y)
    {
        for (int x = 0; x < map.width(); ++x)
        {
            Tile tile = map.at(x, y);

            if (!tile.land)
            {
                tile.terrain = Terrain::ocean;
                map.setTile(x, y, tile);
                continue;
            }

            tile.terrain = pickTerrainForRegion(tile.region, rng);
            map.setTile(x, y, tile);
        }
    }
}

void smoothTerrainPasses(Map& map)
{
    for (int pass = 0; pass < 2; ++pass)
    {
        std::vector<Tile> nextTiles;

        for (int y = 0; y < map.height(); ++y)
        {
            for (int x = 0; x < map.width(); ++x)
            {
                nextTiles.push_back(map.at(x, y));
            }
        }

        for (int y = 1; y < map.height() - 1; ++y)
        {
            for (int x = 1; x < map.width() - 1; ++x)
            {
                const Tile& center = map.at(x, y);

                if (!center.land)
                {
                    continue;
                }

                int counts[kTerrainCount] = {};

                for (const auto& direction : kDiagDirs)
                {
                    const int nextX = x + direction[0];
                    const int nextY = y + direction[1];
                    const Tile& neighbor = map.at(nextX, nextY);

                    if (neighbor.land && neighbor.region == center.region)
                    {
                        ++counts[static_cast<int>(neighbor.terrain)];
                    }
                }

                int bestTerrain = static_cast<int>(center.terrain);
                int bestCount = counts[bestTerrain];

                for (int terrainIndex = 0; terrainIndex < kTerrainCount; ++terrainIndex)
                {
                    if (counts[terrainIndex] > bestCount)
                    {
                        bestTerrain = terrainIndex;
                        bestCount = counts[terrainIndex];
                    }
                }

                if (bestCount >= 4)
                {
                    Tile tile = center;
                    tile.terrain = static_cast<Terrain>(bestTerrain);
                    nextTiles[static_cast<size_t>(y * map.width() + x)] = tile;
                }
            }
        }

        for (int y = 0; y < map.height(); ++y)
        {
            for (int x = 0; x < map.width(); ++x)
            {
                map.setTile(x, y, nextTiles[static_cast<size_t>(y * map.width() + x)]);
            }
        }
    }
}