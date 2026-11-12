// File: map_regions.cpp
// Commit: Split region layout generation into modular efficient layout files

#include "map_internal.h"
#include "map_region_layout_internal.h"

#include <cstdint>
#include <random>

MapState buildRegionLayout(const Map& map, std::uint32_t seed)
{
    std::mt19937 rng(seed);

    MapState state;
    state.seeds = buildRegionSeeds(map.width(), map.height(), rng);
    state.owner.resize(static_cast<size_t>(map.width() * map.height()), Region::none);
    state.landMask.resize(static_cast<size_t>(map.width() * map.height()), false);

    assignRegionSeedTiles(state, map);
    growRegionsEfficient(state, map, seed);
    enforceRegionTileBounds(state, map, seed);
    enforceRegionAdjacencyRules(state, map);

    return state;
}