// File: building/building_names.cpp
// Commit: Split holding, building, and resource display names into dedicated source file

#include "building.h"

const char* holdingTypeName(HoldingType type)
{
    switch (type)
    {
        case HoldingType::castle: return "Castle";
        case HoldingType::city: return "City";
        case HoldingType::church: return "Church";
        case HoldingType::tribe: return "Tribe";
        case HoldingType::sacred_site: return "Sacred Site";
        case HoldingType::noble_estate: return "Noble Estate";
        case HoldingType::merchant_estate: return "Merchant Estate";
        case HoldingType::religious_estate: return "Religious Estate";
        case HoldingType::tribal_estate: return "Tribal Estate";
        case HoldingType::smallholder: return "Smallholder";
        case HoldingType::tribal_smallholder: return "Tribal Smallholder";
        default: return "Unknown";
    }
}

const char* buildingTypeName(BuildingType type)
{
    switch (type)
    {
        case BuildingType::explorer: return "Explorer";
        case BuildingType::game_hunters: return "Game Hunters";
        case BuildingType::fowl_hunters: return "Fowl Hunters";
        case BuildingType::fishermen: return "Fishermen";
        case BuildingType::beachcombers: return "Beachcombers";
        case BuildingType::gatherers: return "Gatherers";
        case BuildingType::loggers: return "Loggers";
        case BuildingType::flint_knappers: return "Flint Knappers";
        case BuildingType::basalt_polishers: return "Basalt Polishers";
        case BuildingType::game_butchers: return "Game Butchers";
        case BuildingType::fowl_butchers: return "Fowl Butchers";
        case BuildingType::feasting: return "Feasting";
        case BuildingType::charcoal_maker: return "Charcoal Maker";
        case BuildingType::potter: return "Potter";
        case BuildingType::tanner: return "Tanner";
        case BuildingType::thatcher: return "Thatcher";
        case BuildingType::ropemaker: return "Ropemaker";
        case BuildingType::deer_hunters: return "Deer Hunters";
        case BuildingType::boar_hunters: return "Boar Hunters";
        case BuildingType::hare_trappers: return "Hare Trappers";
        case BuildingType::beaver_trappers: return "Beaver Trappers";
        case BuildingType::seal_hunters: return "Seal Hunters";
        case BuildingType::whale_hunters: return "Whale Hunters";
        case BuildingType::salmon_fishers: return "Salmon Fishers";
        case BuildingType::river_fishers: return "River Fishers";
        case BuildingType::lake_fishers: return "Lake Fishers";
        case BuildingType::eel_trappers: return "Eel Trappers";
        case BuildingType::shellfish_gatherers: return "Shellfish Gatherers";
        case BuildingType::crab_gatherers: return "Crab Gatherers";
        case BuildingType::egg_gatherers: return "Egg Gatherers";
        case BuildingType::honey_gatherers: return "Honey Gatherers";
        case BuildingType::berry_gatherers: return "Berry Gatherers";
        case BuildingType::nut_gatherers: return "Nut Gatherers";
        case BuildingType::root_gatherers: return "Root Gatherers";
        case BuildingType::mushroom_gatherers: return "Mushroom Gatherers";
        case BuildingType::herb_gatherers: return "Herb Gatherers";
        case BuildingType::reed_cutters: return "Reed Cutters";
        case BuildingType::fiber_gatherers: return "Fiber Gatherers";
        case BuildingType::resin_tappers: return "Resin Tappers";
        case BuildingType::bark_strippers: return "Bark Strippers";
        case BuildingType::clay_diggers: return "Clay Diggers";
        case BuildingType::sand_gatherers: return "Sand Gatherers";
        case BuildingType::limestone_gatherers: return "Limestone Gatherers";
        case BuildingType::granite_quarriers: return "Granite Quarriers";
        case BuildingType::obsidian_knappers: return "Obsidian Knappers";
        case BuildingType::copper_collectors: return "Copper Collectors";
        case BuildingType::bog_iron_collectors: return "Bog Iron Collectors";
        case BuildingType::salt_gatherers: return "Salt Gatherers";
        case BuildingType::peat_cutters: return "Peat Cutters";
        case BuildingType::hide_workers: return "Hide Workers";
        case BuildingType::bone_carvers: return "Bone Carvers";
        case BuildingType::antler_workers: return "Antler Workers";
        case BuildingType::horn_workers: return "Horn Workers";
        case BuildingType::feather_workers: return "Feather Workers";
        case BuildingType::basket_weavers: return "Basket Weavers";
        case BuildingType::mat_weavers: return "Mat Weavers";
        case BuildingType::netmakers: return "Netmakers";
        case BuildingType::shell_workers: return "Shell Workers";
        case BuildingType::stone_polishers: return "Stone Polishers";
        case BuildingType::copper_workers: return "Copper Workers";
        case BuildingType::salt_curers: return "Salt Curers";
        case BuildingType::smokehouses: return "Smokehouses";
        case BuildingType::drying_racks: return "Drying Racks";
        default: return "Unknown";
    }
}

const char* resourceTypeName(ResourceType type)
{
    switch (type)
    {
        case ResourceType::prestige: return "Prestige";
        case ResourceType::wild_game: return "Wild Game";
        case ResourceType::wild_fowl: return "Wild Fowl";
        case ResourceType::meat: return "Meat";
        case ResourceType::fish: return "Fish";
        case ResourceType::shellfish: return "Shellfish";
        case ResourceType::wheat: return "Wheat";
        case ResourceType::barley: return "Barley";
        case ResourceType::fodder: return "Fodder";
        case ResourceType::onion: return "Onion";
        case ResourceType::turnip: return "Turnip";
        case ResourceType::apples: return "Apples";
        case ResourceType::dill: return "Dill";
        case ResourceType::wood: return "Wood";
        case ResourceType::timber: return "Timber";
        case ResourceType::flint: return "Flint";
        case ResourceType::basalt: return "Basalt";
        case ResourceType::bone: return "Bone";
        case ResourceType::antler: return "Antler";
        case ResourceType::horn: return "Horn";
        case ResourceType::hide: return "Hide";
        case ResourceType::leather: return "Leather";
        case ResourceType::lard: return "Lard";
        case ResourceType::feathers: return "Feathers";
        case ResourceType::raw_clay: return "Raw Clay";
        case ResourceType::pottery: return "Pottery";
        case ResourceType::charcoal: return "Charcoal";
        case ResourceType::thatch: return "Thatch";
        case ResourceType::rope: return "Rope";
        case ResourceType::deer: return "Deer";
        case ResourceType::boar: return "Boar";
        case ResourceType::hare: return "Hare";
        case ResourceType::beaver: return "Beaver";
        case ResourceType::seal: return "Seal";
        case ResourceType::whale: return "Whale";
        case ResourceType::salmon: return "Salmon";
        case ResourceType::river_fish: return "River Fish";
        case ResourceType::lake_fish: return "Lake Fish";
        case ResourceType::eel: return "Eel";
        case ResourceType::crab: return "Crab";
        case ResourceType::eggs: return "Eggs";
        case ResourceType::honey: return "Honey";
        case ResourceType::berries: return "Berries";
        case ResourceType::nuts: return "Nuts";
        case ResourceType::roots: return "Roots";
        case ResourceType::mushrooms: return "Mushrooms";
        case ResourceType::herbs: return "Herbs";
        case ResourceType::reeds: return "Reeds";
        case ResourceType::plant_fiber: return "Plant Fiber";
        case ResourceType::resin: return "Resin";
        case ResourceType::bark: return "Bark";
        case ResourceType::sand: return "Sand";
        case ResourceType::limestone: return "Limestone";
        case ResourceType::granite: return "Granite";
        case ResourceType::obsidian: return "Obsidian";
        case ResourceType::native_copper: return "Native Copper";
        case ResourceType::bog_iron: return "Bog Iron";
        case ResourceType::salt: return "Salt";
        case ResourceType::peat: return "Peat";
        case ResourceType::cured_meat: return "Cured Meat";
        case ResourceType::smoked_fish: return "Smoked Fish";
        case ResourceType::dried_fish: return "Dried Fish";
        case ResourceType::dried_berries: return "Dried Berries";
        case ResourceType::baskets: return "Baskets";
        case ResourceType::mats: return "Mats";
        case ResourceType::nets: return "Nets";
        case ResourceType::worked_bone: return "Worked Bone";
        case ResourceType::worked_antler: return "Worked Antler";
        case ResourceType::worked_horn: return "Worked Horn";
        case ResourceType::worked_shell: return "Worked Shell";
        case ResourceType::worked_stone: return "Worked Stone";
        case ResourceType::copper_tools: return "Copper Tools";
        default: return "Unknown";
    }
}