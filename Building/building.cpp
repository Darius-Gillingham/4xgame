// File: building/building.cpp
// Commit: Add expanded tribal sub-building names and tiered sub-building slot capacities

#include "building.h"

const char* buildingTypeName(BuildingType type)
{
    switch (type)
    {
        case BuildingType::castle: return "Castle";
        case BuildingType::city: return "City";
        case BuildingType::church: return "Church";
        case BuildingType::tribe: return "Tribe";
        case BuildingType::noble_estate: return "Noble Estate";
        case BuildingType::merchant_estate: return "Merchant Estate";
        case BuildingType::religious_estate: return "Religious Estate";
        case BuildingType::tribal_estate: return "Tribal Estate";
        case BuildingType::smallholder: return "Smallholder";
        case BuildingType::tribal_smallholder: return "Tribal Smallholder";
        default: return "Unknown";
    }
}

const char* subBuildingTypeName(SubBuildingType type)
{
    switch (type)
    {
        case SubBuildingType::game_hunters: return "Game Hunters";
        case SubBuildingType::fowl_hunters: return "Fowl Hunters";
        case SubBuildingType::fishermen: return "Fishermen";
        case SubBuildingType::beachcombers: return "Beachcombers";
        case SubBuildingType::gatherers: return "Gatherers";
        case SubBuildingType::loggers: return "Loggers";
        case SubBuildingType::flint_knappers: return "Flint Knappers";
        case SubBuildingType::basalt_polishers: return "Basalt Polishers";
        case SubBuildingType::butcher: return "Butcher";
        case SubBuildingType::feasting: return "Feasting";
        case SubBuildingType::charcoal_maker: return "Charcoal Maker";
        case SubBuildingType::potter: return "Potter";
        case SubBuildingType::tanner: return "Tanner";
        case SubBuildingType::thatcher: return "Thatcher";
        case SubBuildingType::ropemaker: return "Ropemaker";
        default: return "Unknown";
    }
}

bool isCapitalBuildingType(BuildingType type)
{
    return
        type == BuildingType::castle ||
        type == BuildingType::city ||
        type == BuildingType::church ||
        type == BuildingType::tribe;
}

bool isRegularBuildingType(BuildingType type)
{
    return !isCapitalBuildingType(type);
}

bool isEstateBuildingType(BuildingType type)
{
    return
        type == BuildingType::noble_estate ||
        type == BuildingType::merchant_estate ||
        type == BuildingType::religious_estate ||
        type == BuildingType::tribal_estate;
}

bool isSmallholderBuildingType(BuildingType type)
{
    return
        type == BuildingType::smallholder ||
        type == BuildingType::tribal_smallholder;
}

bool isRegularCapitalType(BuildingType type)
{
    return
        type == BuildingType::castle ||
        type == BuildingType::city ||
        type == BuildingType::church;
}

bool isTribalCapitalType(BuildingType type)
{
    return type == BuildingType::tribe;
}

bool isRegularSmallholderType(BuildingType type)
{
    return type == BuildingType::smallholder;
}

bool isTribalSmallholderType(BuildingType type)
{
    return type == BuildingType::tribal_smallholder;
}

bool canBuildAsRoot(BuildingType type)
{
    return type == BuildingType::tribe;
}

int directAttachmentSlotCapacity(BuildingType type)
{
    switch (type)
    {
        case BuildingType::castle: return 4;
        case BuildingType::city: return 4;
        case BuildingType::church: return 4;
        case BuildingType::tribe: return 4;

        case BuildingType::noble_estate: return 2;
        case BuildingType::merchant_estate: return 2;
        case BuildingType::religious_estate: return 2;
        case BuildingType::tribal_estate: return 2;

        case BuildingType::smallholder: return 0;
        case BuildingType::tribal_smallholder: return 0;

        default: return 0;
    }
}

int subBuildingSlotCapacity(BuildingType type)
{
    if (isCapitalBuildingType(type))
    {
        return 7;
    }

    if (isEstateBuildingType(type))
    {
        return 5;
    }

    if (isSmallholderBuildingType(type))
    {
        return 3;
    }

    return 0;
}

bool canHostAttachedBuildings(BuildingType type)
{
    return directAttachmentSlotCapacity(type) > 0;
}

bool canHostSubBuildings(BuildingType type)
{
    return subBuildingSlotCapacity(type) > 0;
}

bool canBuildTribalSubBuilding(BuildingType parentType, SubBuildingType subBuildingType)
{
    if (
        parentType != BuildingType::tribe &&
        parentType != BuildingType::tribal_estate &&
        parentType != BuildingType::tribal_smallholder
    )
    {
        return false;
    }

    switch (subBuildingType)
    {
        case SubBuildingType::game_hunters:
        case SubBuildingType::fowl_hunters:
        case SubBuildingType::fishermen:
        case SubBuildingType::beachcombers:
        case SubBuildingType::gatherers:
        case SubBuildingType::loggers:
        case SubBuildingType::flint_knappers:
        case SubBuildingType::basalt_polishers:
        case SubBuildingType::butcher:
        case SubBuildingType::feasting:
        case SubBuildingType::charcoal_maker:
        case SubBuildingType::potter:
        case SubBuildingType::tanner:
        case SubBuildingType::thatcher:
        case SubBuildingType::ropemaker:
            return true;

        default:
            return false;
    }
}

bool canEstateAttachToParent(BuildingType estateType, BuildingType parentType)
{
    if (!isEstateBuildingType(estateType) || !canHostAttachedBuildings(parentType))
    {
        return false;
    }

    if (parentType == BuildingType::tribe) return estateType == BuildingType::tribal_estate;

    return false;
}

bool canSmallholderAttachToParent(BuildingType smallholderType, BuildingType parentType)
{
    if (!isSmallholderBuildingType(smallholderType) || !canHostAttachedBuildings(parentType))
    {
        return false;
    }

    if (isTribalCapitalType(parentType) || parentType == BuildingType::tribal_estate)
    {
        return smallholderType == BuildingType::tribal_smallholder;
    }

    return false;
}

std::vector<BuildingType> allowedRootBuildingTypes()
{
    return {
        BuildingType::tribe
    };
}

std::vector<BuildingType> allowedEstateTypesForParent(BuildingType parentType)
{
    switch (parentType)
    {
        case BuildingType::tribe: return {BuildingType::tribal_estate};
        default: return {};
    }
}

std::vector<BuildingType> allowedSmallholderTypesForParent(BuildingType parentType)
{
    if (isTribalCapitalType(parentType) || parentType == BuildingType::tribal_estate)
    {
        return {BuildingType::tribal_smallholder};
    }

    return {};
}

std::vector<SubBuildingType> allowedSubBuildingTypesForParent(BuildingType parentType)
{
    std::vector<SubBuildingType> allowedTypes;

    const std::vector<SubBuildingType> tribalTypes = {
        SubBuildingType::game_hunters,
        SubBuildingType::fowl_hunters,
        SubBuildingType::fishermen,
        SubBuildingType::beachcombers,
        SubBuildingType::gatherers,
        SubBuildingType::loggers,
        SubBuildingType::flint_knappers,
        SubBuildingType::basalt_polishers,
        SubBuildingType::butcher,
        SubBuildingType::feasting,
        SubBuildingType::charcoal_maker,
        SubBuildingType::potter,
        SubBuildingType::tanner,
        SubBuildingType::thatcher,
        SubBuildingType::ropemaker
    };

    for (SubBuildingType type : tribalTypes)
    {
        if (canBuildTribalSubBuilding(parentType, type))
        {
            allowedTypes.push_back(type);
        }
    }

    return allowedTypes;
}

std::string buildingDisplayName(const Building& building)
{
    return std::string(buildingTypeName(building.type)) + " #" + std::to_string(building.id);
}

std::string subBuildingDisplayName(const SubBuilding& subBuilding)
{
    return std::string(subBuildingTypeName(subBuilding.type)) + " #" + std::to_string(subBuilding.id);
}