// File: building.h
// Commit: Add tribal sub-building types and storage to building records

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

enum class SubBuildingType
{
    hunter_tent,
    gatherer_tent,
    chief_tent
};

const char* buildingTypeName(BuildingType type);
const char* subBuildingTypeName(SubBuildingType type);

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

bool canBuildTribalSubBuilding(BuildingType parentType, SubBuildingType subBuildingType);

bool canEstateAttachToParent(BuildingType estateType, BuildingType parentType);
bool canSmallholderAttachToParent(BuildingType smallholderType, BuildingType parentType);

std::vector<BuildingType> allowedRootBuildingTypes();
std::vector<BuildingType> allowedEstateTypesForParent(BuildingType parentType);
std::vector<BuildingType> allowedSmallholderTypesForParent(BuildingType parentType);
std::vector<SubBuildingType> allowedSubBuildingTypesForParent(BuildingType parentType);

struct SubBuilding
{
    int id = -1;
    SubBuildingType type = SubBuildingType::hunter_tent;
};

struct Building
{
    int id = -1;
    BuildingType type = BuildingType::castle;
    int parentId = -1;
    std::vector<int> childIds;
    std::vector<SubBuilding> subBuildings;
};

std::string buildingDisplayName(const Building& building);
std::string subBuildingDisplayName(const SubBuilding& subBuilding);