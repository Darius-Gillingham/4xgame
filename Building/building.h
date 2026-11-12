// File: building/building.h
// Commit: Add Tribal Band and Piety while removing the obsolete Explorer building

#pragma once

#include <string>
#include <vector>

enum class HoldingType
{
    castle,
    city,
    church,
    tribe,
    tribal_band,
    sacred_site,
    noble_estate,
    merchant_estate,
    religious_estate,
    tribal_estate,
    smallholder,
    tribal_smallholder
};

enum class BuildingType
{
    game_hunters,
    fowl_hunters,
    fishermen,
    beachcombers,
    gatherers,
    loggers,
    flint_knappers,
    basalt_polishers,
    game_butchers,
    fowl_butchers,
    feasting,
    charcoal_maker,
    potter,
    tanner,
    thatcher,
    ropemaker,

    deer_hunters,
    boar_hunters,
    hare_trappers,
    beaver_trappers,
    seal_hunters,
    whale_hunters,
    salmon_fishers,
    river_fishers,
    lake_fishers,
    eel_trappers,
    shellfish_gatherers,
    crab_gatherers,
    egg_gatherers,
    honey_gatherers,
    berry_gatherers,
    nut_gatherers,
    root_gatherers,
    mushroom_gatherers,
    herb_gatherers,
    reed_cutters,
    fiber_gatherers,
    resin_tappers,
    bark_strippers,
    clay_diggers,
    sand_gatherers,
    limestone_gatherers,
    granite_quarriers,
    obsidian_knappers,
    copper_collectors,
    bog_iron_collectors,
    salt_gatherers,
    peat_cutters,
    hide_workers,
    bone_carvers,
    antler_workers,
    horn_workers,
    feather_workers,
    basket_weavers,
    mat_weavers,
    netmakers,
    shell_workers,
    stone_polishers,
    copper_workers,
    salt_curers,
    smokehouses,
    drying_racks
};

enum class ResourceType
{
    prestige,
    piety,
    wild_game,
    wild_fowl,
    meat,
    fish,
    shellfish,
    wheat,
    barley,
    fodder,
    onion,
    turnip,
    apples,
    dill,
    wood,
    timber,
    flint,
    basalt,
    bone,
    antler,
    horn,
    hide,
    leather,
    lard,
    feathers,
    raw_clay,
    pottery,
    charcoal,
    thatch,
    rope,

    deer,
    boar,
    hare,
    beaver,
    seal,
    whale,
    salmon,
    river_fish,
    lake_fish,
    eel,
    crab,
    eggs,
    honey,
    berries,
    nuts,
    roots,
    mushrooms,
    herbs,
    reeds,
    plant_fiber,
    resin,
    bark,
    sand,
    limestone,
    granite,
    obsidian,
    native_copper,
    bog_iron,
    salt,
    peat,
    cured_meat,
    smoked_fish,
    dried_fish,
    dried_berries,
    baskets,
    mats,
    nets,
    worked_bone,
    worked_antler,
    worked_horn,
    worked_shell,
    worked_stone,
    copper_tools
};

struct ResourceAmount
{
    ResourceType type = ResourceType::prestige;
    int amount = 0;
};

struct FoodProfile
{
    int fat = 0;
    int carbs = 0;
    int protein = 0;
    int luxury = 0;
};

struct BuildingWorkProfile
{
    int jobCapacity = 10;
    std::vector<ResourceAmount> inputs;
    std::vector<ResourceAmount> outputs;
};

struct BuildingTier
{
    int level = 1;
    int count = 0;
    int employedJobs = 0;
};

struct Levy
{
    int slotIndex = 0;
    bool raised = false;
};

const char* holdingTypeName(HoldingType type);
const char* buildingTypeName(BuildingType type);
const char* resourceTypeName(ResourceType type);

BuildingWorkProfile workProfileForBuilding(BuildingType type);
FoodProfile foodProfileForResource(ResourceType type);
bool isFoodResource(ResourceType type);

bool isCapitalHoldingType(HoldingType type);
bool isRegularHoldingType(HoldingType type);
bool isEstateHoldingType(HoldingType type);
bool isSmallholderHoldingType(HoldingType type);

bool isRegularCapitalType(HoldingType type);
bool isTribalCapitalType(HoldingType type);

bool isRegularSmallholderType(HoldingType type);
bool isTribalSmallholderType(HoldingType type);

bool canBuildAsRoot(HoldingType type);

int directAttachmentSlotCapacity(HoldingType type);
int buildingSlotCapacity(HoldingType type);

bool canHostAttachedHoldings(HoldingType type);
bool canHostBuildings(HoldingType type);

bool canBuildTribalBuilding(HoldingType parentType, BuildingType buildingType);

bool canEstateAttachToParent(HoldingType estateType, HoldingType parentType);
bool canSmallholderAttachToParent(HoldingType smallholderType, HoldingType parentType);

std::vector<HoldingType> allowedRootHoldingTypes();
std::vector<HoldingType> allowedEstateTypesForParent(HoldingType parentType);
std::vector<HoldingType> allowedSmallholderTypesForParent(HoldingType parentType);
std::vector<BuildingType> allowedBuildingTypesForParent(HoldingType parentType);

struct Building
{
    int id = -1;
    BuildingType type = BuildingType::game_hunters;
    std::vector<BuildingTier> tiers;
    std::vector<Levy> levies{
        {0, false},
        {1, false}
    };
};

struct Holding
{
    int id = -1;
    HoldingType type = HoldingType::castle;
    int parentId = -1;
    std::vector<int> childIds;
    std::vector<Building> buildings;
};

std::string holdingDisplayName(const Holding& holding);
std::string buildingDisplayName(const Building& building);