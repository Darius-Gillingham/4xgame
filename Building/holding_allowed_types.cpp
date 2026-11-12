// File: building/holding_allowed_types.cpp
// Commit: Add Tribal Band as an early root holding and remove the obsolete Explorer building

#include "building.h"

std::vector<HoldingType> allowedRootHoldingTypes()
{
    return {
        HoldingType::tribal_band,
        HoldingType::sacred_site
    };
}

std::vector<HoldingType> allowedEstateTypesForParent(HoldingType parentType)
{
    switch (parentType)
    {
        case HoldingType::tribe:
            return {
                HoldingType::tribal_estate
            };

        default:
            return {};
    }
}

std::vector<HoldingType> allowedSmallholderTypesForParent(HoldingType parentType)
{
    if (
        isTribalCapitalType(parentType) ||
        parentType == HoldingType::tribal_estate
    )
    {
        return {
            HoldingType::tribal_smallholder
        };
    }

    return {};
}

std::vector<BuildingType> allowedBuildingTypesForParent(HoldingType parentType)
{
    std::vector<BuildingType> allowedTypes;

    const std::vector<BuildingType> tribalTypes = {
        BuildingType::game_hunters,
        BuildingType::fowl_hunters,
        BuildingType::fishermen,
        BuildingType::beachcombers,
        BuildingType::gatherers,
        BuildingType::loggers,
        BuildingType::flint_knappers,
        BuildingType::basalt_polishers,
        BuildingType::game_butchers,
        BuildingType::fowl_butchers,
        BuildingType::feasting,
        BuildingType::charcoal_maker,
        BuildingType::potter,
        BuildingType::tanner,
        BuildingType::thatcher,
        BuildingType::ropemaker,
        BuildingType::deer_hunters,
        BuildingType::boar_hunters,
        BuildingType::hare_trappers,
        BuildingType::beaver_trappers,
        BuildingType::seal_hunters,
        BuildingType::whale_hunters,
        BuildingType::salmon_fishers,
        BuildingType::river_fishers,
        BuildingType::lake_fishers,
        BuildingType::eel_trappers,
        BuildingType::shellfish_gatherers,
        BuildingType::crab_gatherers,
        BuildingType::egg_gatherers,
        BuildingType::honey_gatherers,
        BuildingType::berry_gatherers,
        BuildingType::nut_gatherers,
        BuildingType::root_gatherers,
        BuildingType::mushroom_gatherers,
        BuildingType::herb_gatherers,
        BuildingType::reed_cutters,
        BuildingType::fiber_gatherers,
        BuildingType::resin_tappers,
        BuildingType::bark_strippers,
        BuildingType::clay_diggers,
        BuildingType::sand_gatherers,
        BuildingType::limestone_gatherers,
        BuildingType::granite_quarriers,
        BuildingType::obsidian_knappers,
        BuildingType::copper_collectors,
        BuildingType::bog_iron_collectors,
        BuildingType::salt_gatherers,
        BuildingType::peat_cutters,
        BuildingType::hide_workers,
        BuildingType::bone_carvers,
        BuildingType::antler_workers,
        BuildingType::horn_workers,
        BuildingType::feather_workers,
        BuildingType::basket_weavers,
        BuildingType::mat_weavers,
        BuildingType::netmakers,
        BuildingType::shell_workers,
        BuildingType::stone_polishers,
        BuildingType::copper_workers,
        BuildingType::salt_curers,
        BuildingType::smokehouses,
        BuildingType::drying_racks
    };

    for (BuildingType type : tribalTypes)
    {
        if (canBuildTribalBuilding(parentType, type))
        {
            allowedTypes.push_back(type);
        }
    }

    return allowedTypes;
}