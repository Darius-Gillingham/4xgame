// File: building/holding_rules.cpp
// Commit: Rename Band to Tribal Band and keep Explorer removed from tribal building rules

#include "building.h"

bool canBuildTribalBuilding(HoldingType parentType, BuildingType buildingType)
{
    if (
        parentType != HoldingType::tribe &&
        parentType != HoldingType::tribal_band &&
        parentType != HoldingType::sacred_site &&
        parentType != HoldingType::tribal_estate &&
        parentType != HoldingType::tribal_smallholder
    )
    {
        return false;
    }

    switch (buildingType)
    {
        case BuildingType::game_hunters:
        case BuildingType::fowl_hunters:
        case BuildingType::fishermen:
        case BuildingType::beachcombers:
        case BuildingType::gatherers:
        case BuildingType::loggers:
        case BuildingType::flint_knappers:
        case BuildingType::basalt_polishers:
        case BuildingType::game_butchers:
        case BuildingType::fowl_butchers:
        case BuildingType::feasting:
        case BuildingType::charcoal_maker:
        case BuildingType::potter:
        case BuildingType::tanner:
        case BuildingType::thatcher:
        case BuildingType::ropemaker:
        case BuildingType::deer_hunters:
        case BuildingType::boar_hunters:
        case BuildingType::hare_trappers:
        case BuildingType::beaver_trappers:
        case BuildingType::seal_hunters:
        case BuildingType::whale_hunters:
        case BuildingType::salmon_fishers:
        case BuildingType::river_fishers:
        case BuildingType::lake_fishers:
        case BuildingType::eel_trappers:
        case BuildingType::shellfish_gatherers:
        case BuildingType::crab_gatherers:
        case BuildingType::egg_gatherers:
        case BuildingType::honey_gatherers:
        case BuildingType::berry_gatherers:
        case BuildingType::nut_gatherers:
        case BuildingType::root_gatherers:
        case BuildingType::mushroom_gatherers:
        case BuildingType::herb_gatherers:
        case BuildingType::reed_cutters:
        case BuildingType::fiber_gatherers:
        case BuildingType::resin_tappers:
        case BuildingType::bark_strippers:
        case BuildingType::clay_diggers:
        case BuildingType::sand_gatherers:
        case BuildingType::limestone_gatherers:
        case BuildingType::granite_quarriers:
        case BuildingType::obsidian_knappers:
        case BuildingType::copper_collectors:
        case BuildingType::bog_iron_collectors:
        case BuildingType::salt_gatherers:
        case BuildingType::peat_cutters:
        case BuildingType::hide_workers:
        case BuildingType::bone_carvers:
        case BuildingType::antler_workers:
        case BuildingType::horn_workers:
        case BuildingType::feather_workers:
        case BuildingType::basket_weavers:
        case BuildingType::mat_weavers:
        case BuildingType::netmakers:
        case BuildingType::shell_workers:
        case BuildingType::stone_polishers:
        case BuildingType::copper_workers:
        case BuildingType::salt_curers:
        case BuildingType::smokehouses:
        case BuildingType::drying_racks:
            return true;

        default:
            return false;
    }
}

bool canEstateAttachToParent(HoldingType estateType, HoldingType parentType)
{
    if (!isEstateHoldingType(estateType) || !canHostAttachedHoldings(parentType))
    {
        return false;
    }

    if (parentType == HoldingType::tribe)
    {
        return estateType == HoldingType::tribal_estate;
    }

    return false;
}

bool canSmallholderAttachToParent(HoldingType smallholderType, HoldingType parentType)
{
    if (!isSmallholderHoldingType(smallholderType) || !canHostAttachedHoldings(parentType))
    {
        return false;
    }

    if (isTribalCapitalType(parentType) || parentType == HoldingType::tribal_estate)
    {
        return smallholderType == HoldingType::tribal_smallholder;
    }

    return false;
}