// File: building.h
// Commit: Redefine building rules so capitals are distinct, regular buildings can be attached or unattached, and all buildings support sub-building slots

#pragma once

#include <string>
#include <vector>

enum class BuildingType
{
    castle,
    city,
    church,
    tribe,
    noble_estate,
    merchant_estate,
    religious_estate,
    tribal_estate,
    smallholder,
    tribal_smallholder
};

const char* buildingTypeName(BuildingType type);

bool isCapitalBuildingType(BuildingType type);
bool isRegularBuildingType(BuildingType type);
bool isEstateBuildingType(BuildingType type);
bool isSmallholderBuildingType(BuildingType type);

bool isRegularCapitalType(BuildingType type);
bool isTribalCapitalType(BuildingType type);

bool isRegularSmallholderType(BuildingType type);
bool isTribalSmallholderType(BuildingType type);

bool canBuildAsRoot(BuildingType type);

int directAttachmentSlotCapacity(BuildingType type);
int subBuildingSlotCapacity(BuildingType type);

bool canHostAttachedBuildings(BuildingType type);
bool canHostSubBuildings(BuildingType type);

bool canEstateAttachToParent(BuildingType estateType, BuildingType parentType);
bool canSmallholderAttachToParent(BuildingType smallholderType, BuildingType parentType);

std::vector<BuildingType> allowedRootBuildingTypes();
std::vector<BuildingType> allowedEstateTypesForParent(BuildingType parentType);
std::vector<BuildingType> allowedSmallholderTypesForParent(BuildingType parentType);

struct Building
{
    int id = -1;
    BuildingType type = BuildingType::castle;
    int parentId = -1;
    std::vector<int> childIds;
};

std::string buildingDisplayName(const Building& building);