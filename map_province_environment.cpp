// File: map_province_environment.cpp
// Commit: Rename available sub-buildings to buildings for the holding and building system

#include "map_province_environment.h"

namespace
{
    void addTerrainCount(
        std::vector<ProvinceTerrainCount>& counts,
        Terrain terrain)
    {
        for (ProvinceTerrainCount& count : counts)
        {
            if (count.terrain == terrain)
            {
                ++count.tileCount;
                return;
            }
        }

        ProvinceTerrainCount count;
        count.terrain = terrain;
        count.tileCount = 1;
        counts.push_back(count);
    }

    void addRegionCount(
        std::vector<ProvinceRegionCount>& counts,
        Region region)
    {
        for (ProvinceRegionCount& count : counts)
        {
            if (count.region == region)
            {
                ++count.tileCount;
                return;
            }
        }

        ProvinceRegionCount count;
        count.region = region;
        count.tileCount = 1;
        counts.push_back(count);
    }

    void addEcologyCapacity(
        std::vector<ProvinceEcologyCapacity>& capacities,
        ProvinceEcologyType type,
        float value)
    {
        if (value <= 0.0f)
        {
            return;
        }

        for (ProvinceEcologyCapacity& capacity : capacities)
        {
            if (capacity.type == type)
            {
                capacity.totalValue += value;
                return;
            }
        }

        ProvinceEcologyCapacity capacity;
        capacity.type = type;
        capacity.totalValue = value;
        capacity.usedValue = 0.0f;
        capacities.push_back(capacity);
    }

    void addAvailableResource(
        std::vector<ResourceType>& resources,
        ResourceType type)
    {
        for (ResourceType existingType : resources)
        {
            if (existingType == type)
            {
                return;
            }
        }

        resources.push_back(type);
    }

    void addAvailableBuilding(
        std::vector<BuildingType>& buildings,
        BuildingType type)
    {
        for (BuildingType existingType : buildings)
        {
            if (existingType == type)
            {
                return;
            }
        }

        buildings.push_back(type);
    }

    void addEcologyAvailability(
        ProvinceEnvironmentSummary& summary,
        ProvinceEcologyType type)
    {
        switch (type)
        {
            case ProvinceEcologyType::tundra:
                addAvailableResource(summary.availableResources, ResourceType::wild_game);
                addAvailableResource(summary.availableResources, ResourceType::wild_fowl);
                addAvailableResource(summary.availableResources, ResourceType::berries);
                addAvailableResource(summary.availableResources, ResourceType::roots);
                addAvailableResource(summary.availableResources, ResourceType::hide);
                addAvailableBuilding(summary.availableBuildings, BuildingType::game_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::fowl_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::berry_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::root_gatherers);
                break;

            case ProvinceEcologyType::boreal:
                addAvailableResource(summary.availableResources, ResourceType::wild_game);
                addAvailableResource(summary.availableResources, ResourceType::wild_fowl);
                addAvailableResource(summary.availableResources, ResourceType::wood);
                addAvailableResource(summary.availableResources, ResourceType::timber);
                addAvailableResource(summary.availableResources, ResourceType::berries);
                addAvailableResource(summary.availableResources, ResourceType::mushrooms);
                addAvailableResource(summary.availableResources, ResourceType::resin);
                addAvailableResource(summary.availableResources, ResourceType::bark);
                addAvailableBuilding(summary.availableBuildings, BuildingType::game_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::fowl_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::loggers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::berry_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::mushroom_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::resin_tappers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::bark_strippers);
                break;

            case ProvinceEcologyType::woods:
                addAvailableResource(summary.availableResources, ResourceType::wild_game);
                addAvailableResource(summary.availableResources, ResourceType::wild_fowl);
                addAvailableResource(summary.availableResources, ResourceType::wood);
                addAvailableResource(summary.availableResources, ResourceType::timber);
                addAvailableResource(summary.availableResources, ResourceType::berries);
                addAvailableResource(summary.availableResources, ResourceType::nuts);
                addAvailableResource(summary.availableResources, ResourceType::mushrooms);
                addAvailableResource(summary.availableResources, ResourceType::herbs);
                addAvailableResource(summary.availableResources, ResourceType::resin);
                addAvailableResource(summary.availableResources, ResourceType::bark);
                addAvailableBuilding(summary.availableBuildings, BuildingType::game_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::fowl_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::loggers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::berry_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::nut_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::mushroom_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::herb_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::resin_tappers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::bark_strippers);
                break;

            case ProvinceEcologyType::jungle:
            case ProvinceEcologyType::jungle_highlands:
                addAvailableResource(summary.availableResources, ResourceType::wild_game);
                addAvailableResource(summary.availableResources, ResourceType::wild_fowl);
                addAvailableResource(summary.availableResources, ResourceType::wood);
                addAvailableResource(summary.availableResources, ResourceType::timber);
                addAvailableResource(summary.availableResources, ResourceType::plant_fiber);
                addAvailableResource(summary.availableResources, ResourceType::resin);
                addAvailableResource(summary.availableResources, ResourceType::herbs);
                addAvailableBuilding(summary.availableBuildings, BuildingType::game_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::fowl_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::loggers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::fiber_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::resin_tappers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::herb_gatherers);
                break;

            case ProvinceEcologyType::highlands:
            case ProvinceEcologyType::hills:
                addAvailableResource(summary.availableResources, ResourceType::wild_game);
                addAvailableResource(summary.availableResources, ResourceType::wild_fowl);
                addAvailableResource(summary.availableResources, ResourceType::flint);
                addAvailableResource(summary.availableResources, ResourceType::basalt);
                addAvailableResource(summary.availableResources, ResourceType::granite);
                addAvailableResource(summary.availableResources, ResourceType::herbs);
                addAvailableBuilding(summary.availableBuildings, BuildingType::game_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::fowl_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::flint_knappers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::basalt_polishers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::granite_quarriers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::herb_gatherers);
                break;

            case ProvinceEcologyType::mountains:
                addAvailableResource(summary.availableResources, ResourceType::wild_game);
                addAvailableResource(summary.availableResources, ResourceType::flint);
                addAvailableResource(summary.availableResources, ResourceType::basalt);
                addAvailableResource(summary.availableResources, ResourceType::granite);
                addAvailableResource(summary.availableResources, ResourceType::obsidian);
                addAvailableResource(summary.availableResources, ResourceType::native_copper);
                addAvailableBuilding(summary.availableBuildings, BuildingType::game_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::flint_knappers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::basalt_polishers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::granite_quarriers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::obsidian_knappers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::copper_collectors);
                break;

            case ProvinceEcologyType::meadow:
            case ProvinceEcologyType::plains:
                addAvailableResource(summary.availableResources, ResourceType::wild_game);
                addAvailableResource(summary.availableResources, ResourceType::wild_fowl);
                addAvailableResource(summary.availableResources, ResourceType::roots);
                addAvailableResource(summary.availableResources, ResourceType::herbs);
                addAvailableResource(summary.availableResources, ResourceType::plant_fiber);
                addAvailableBuilding(summary.availableBuildings, BuildingType::game_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::fowl_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::root_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::herb_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::fiber_gatherers);
                break;

            case ProvinceEcologyType::marsh:
            case ProvinceEcologyType::floodplain:
                addAvailableResource(summary.availableResources, ResourceType::wild_fowl);
                addAvailableResource(summary.availableResources, ResourceType::fish);
                addAvailableResource(summary.availableResources, ResourceType::river_fish);
                addAvailableResource(summary.availableResources, ResourceType::eel);
                addAvailableResource(summary.availableResources, ResourceType::eggs);
                addAvailableResource(summary.availableResources, ResourceType::reeds);
                addAvailableResource(summary.availableResources, ResourceType::raw_clay);
                addAvailableResource(summary.availableResources, ResourceType::bog_iron);
                addAvailableBuilding(summary.availableBuildings, BuildingType::fowl_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::fishermen);
                addAvailableBuilding(summary.availableBuildings, BuildingType::river_fishers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::eel_trappers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::egg_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::reed_cutters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::clay_diggers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::bog_iron_collectors);
                break;

            case ProvinceEcologyType::steppe:
            case ProvinceEcologyType::savanna:
                addAvailableResource(summary.availableResources, ResourceType::wild_game);
                addAvailableResource(summary.availableResources, ResourceType::wild_fowl);
                addAvailableResource(summary.availableResources, ResourceType::roots);
                addAvailableResource(summary.availableResources, ResourceType::herbs);
                addAvailableResource(summary.availableResources, ResourceType::hide);
                addAvailableBuilding(summary.availableBuildings, BuildingType::game_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::fowl_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::root_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::herb_gatherers);
                break;

            case ProvinceEcologyType::arid:
            case ProvinceEcologyType::desert:
                addAvailableResource(summary.availableResources, ResourceType::wild_game);
                addAvailableResource(summary.availableResources, ResourceType::roots);
                addAvailableResource(summary.availableResources, ResourceType::salt);
                addAvailableResource(summary.availableResources, ResourceType::sand);
                addAvailableBuilding(summary.availableBuildings, BuildingType::game_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::root_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::salt_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::sand_gatherers);
                break;

            case ProvinceEcologyType::oasis:
                addAvailableResource(summary.availableResources, ResourceType::wild_fowl);
                addAvailableResource(summary.availableResources, ResourceType::roots);
                addAvailableResource(summary.availableResources, ResourceType::herbs);
                addAvailableResource(summary.availableResources, ResourceType::raw_clay);
                addAvailableBuilding(summary.availableBuildings, BuildingType::fowl_hunters);
                addAvailableBuilding(summary.availableBuildings, BuildingType::root_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::herb_gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::clay_diggers);
                break;

            case ProvinceEcologyType::farmlands:
                addAvailableResource(summary.availableResources, ResourceType::wheat);
                addAvailableResource(summary.availableResources, ResourceType::barley);
                addAvailableResource(summary.availableResources, ResourceType::fodder);
                addAvailableResource(summary.availableResources, ResourceType::onion);
                addAvailableResource(summary.availableResources, ResourceType::turnip);
                addAvailableResource(summary.availableResources, ResourceType::raw_clay);
                addAvailableBuilding(summary.availableBuildings, BuildingType::gatherers);
                addAvailableBuilding(summary.availableBuildings, BuildingType::clay_diggers);
                break;

            default:
                break;
        }
    }
}

const char* provinceEcologyTypeName(ProvinceEcologyType type)
{
    switch (type)
    {
        case ProvinceEcologyType::tundra: return "Tundra";
        case ProvinceEcologyType::boreal: return "Boreal";
        case ProvinceEcologyType::woods: return "Woods";
        case ProvinceEcologyType::jungle: return "Jungle";
        case ProvinceEcologyType::jungle_highlands: return "Jungle Highlands";
        case ProvinceEcologyType::highlands: return "Highlands";
        case ProvinceEcologyType::hills: return "Hills";
        case ProvinceEcologyType::mountains: return "Mountains";
        case ProvinceEcologyType::meadow: return "Meadow";
        case ProvinceEcologyType::plains: return "Plains";
        case ProvinceEcologyType::marsh: return "Marsh";
        case ProvinceEcologyType::floodplain: return "Floodplain";
        case ProvinceEcologyType::steppe: return "Steppe";
        case ProvinceEcologyType::savanna: return "Savanna";
        case ProvinceEcologyType::arid: return "Arid";
        case ProvinceEcologyType::desert: return "Desert";
        case ProvinceEcologyType::oasis: return "Oasis";
        case ProvinceEcologyType::farmlands: return "Farmlands";
        default: return "Unknown Ecology";
    }
}

ProvinceEcologyType provinceEcologyTypeForTerrain(Terrain terrain)
{
    switch (terrain)
    {
        case Terrain::tundra: return ProvinceEcologyType::tundra;
        case Terrain::boreal: return ProvinceEcologyType::boreal;
        case Terrain::woods: return ProvinceEcologyType::woods;
        case Terrain::jungle: return ProvinceEcologyType::jungle;
        case Terrain::jungle_highlands: return ProvinceEcologyType::jungle_highlands;
        case Terrain::highlands: return ProvinceEcologyType::highlands;
        case Terrain::hills: return ProvinceEcologyType::hills;
        case Terrain::mountains: return ProvinceEcologyType::mountains;
        case Terrain::meadow: return ProvinceEcologyType::meadow;
        case Terrain::plains: return ProvinceEcologyType::plains;
        case Terrain::marsh: return ProvinceEcologyType::marsh;
        case Terrain::floodplain: return ProvinceEcologyType::floodplain;
        case Terrain::steppe: return ProvinceEcologyType::steppe;
        case Terrain::savanna: return ProvinceEcologyType::savanna;
        case Terrain::arid: return ProvinceEcologyType::arid;
        case Terrain::desert: return ProvinceEcologyType::desert;
        case Terrain::oasis: return ProvinceEcologyType::oasis;
        case Terrain::farmlands: return ProvinceEcologyType::farmlands;
        default: return ProvinceEcologyType::meadow;
    }
}

float provinceEcologyValueForTerrain(Terrain terrain)
{
    switch (terrain)
    {
        case Terrain::tundra: return 1.0f;
        case Terrain::boreal: return 1.0f;
        case Terrain::woods: return 1.0f;
        case Terrain::jungle: return 1.0f;
        case Terrain::jungle_highlands: return 1.0f;
        case Terrain::highlands: return 1.0f;
        case Terrain::hills: return 1.0f;
        case Terrain::mountains: return 1.0f;
        case Terrain::meadow: return 1.0f;
        case Terrain::plains: return 1.0f;
        case Terrain::marsh: return 1.0f;
        case Terrain::floodplain: return 1.0f;
        case Terrain::steppe: return 1.0f;
        case Terrain::savanna: return 1.0f;
        case Terrain::arid: return 1.0f;
        case Terrain::desert: return 1.0f;
        case Terrain::oasis: return 1.0f;
        case Terrain::farmlands: return 1.0f;
        default: return 0.0f;
    }
}

ProvinceEnvironmentSummary summarizeProvinceEnvironment(
    const Map& map,
    int provinceId)
{
    ProvinceEnvironmentSummary summary;
    summary.provinceId = provinceId;

    if (provinceId < 0)
    {
        return summary;
    }

    for (int y = 0; y < map.height(); ++y)
    {
        for (int x = 0; x < map.width(); ++x)
        {
            const Tile& tile = map.at(x, y);

            if (!tile.land || tile.provinceId != provinceId)
            {
                continue;
            }

            const ProvinceEcologyType ecologyType =
                provinceEcologyTypeForTerrain(tile.terrain);

            ++summary.totalTiles;
            ++summary.landTiles;

            addTerrainCount(summary.terrainCounts, tile.terrain);
            addRegionCount(summary.regionCounts, tile.region);
            addEcologyCapacity(
                summary.ecologyCapacities,
                ecologyType,
                provinceEcologyValueForTerrain(tile.terrain)
            );
            addEcologyAvailability(summary, ecologyType);
        }
    }

    return summary;
}