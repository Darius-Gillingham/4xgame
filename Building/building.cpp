// File: building.cpp
// Commit: Define tribal sub-building names and parent rules

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
        case SubBuildingType::hunter_tent: return "Hunter Tent";
        case SubBuildingType::gatherer_tent: return "Gatherer Tent";
        case SubBuildingType::chief_tent: return "Chief Tent";
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
    return true;
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

int subBuildingSlotCapacity(BuildingType)
{
    return 9;
}

bool canHostAttachedBuildings(BuildingType type)
{
    return directAttachmentSlotCapacity(type) > 0;
}

bool canHostSubBuildings(BuildingType)
{
    return true;
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
        case SubBuildingType::hunter_tent:
        case SubBuildingType::gatherer_tent:
            return true;

        case SubBuildingType::chief_tent:
            return parentType == BuildingType::tribe || parentType == BuildingType::tribal_estate;

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

    if (parentType == BuildingType::castle) return estateType == BuildingType::noble_estate;
    if (parentType == BuildingType::city) return estateType == BuildingType::merchant_estate;
    if (parentType == BuildingType::church) return estateType == BuildingType::religious_estate;
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

    return smallholderType == BuildingType::smallholder;
}

std::vector<BuildingType> allowedRootBuildingTypes()
{
    return {
        BuildingType::castle,
        BuildingType::city,
        BuildingType::church,
        BuildingType::tribe,
        BuildingType::noble_estate,
        BuildingType::merchant_estate,
        BuildingType::religious_estate,
        BuildingType::tribal_estate,
        BuildingType::smallholder,
        BuildingType::tribal_smallholder
    };
}

std::vector<BuildingType> allowedEstateTypesForParent(BuildingType parentType)
{
    switch (parentType)
    {
        case BuildingType::castle: return {BuildingType::noble_estate};
        case BuildingType::city: return {BuildingType::merchant_estate};
        case BuildingType::church: return {BuildingType::religious_estate};
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

    return {BuildingType::smallholder};
}

std::vector<SubBuildingType> allowedSubBuildingTypesForParent(BuildingType parentType)
{
    std::vector<SubBuildingType> allowedTypes;

    const std::vector<SubBuildingType> tribalTypes = {
        SubBuildingType::hunter_tent,
        SubBuildingType::gatherer_tent,
        SubBuildingType::chief_tent
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