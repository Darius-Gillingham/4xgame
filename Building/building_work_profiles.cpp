// File: building/building_work_profiles.cpp
// Commit: Split building work profiles into dedicated source file

#include "building.h"

BuildingWorkProfile workProfileForBuilding(BuildingType type)
{
    BuildingWorkProfile profile;
    profile.jobCapacity = 10;

    switch (type)
    {
        case BuildingType::explorer:
            break;

        case BuildingType::game_hunters:
            profile.outputs.push_back({ResourceType::wild_game, 10});
            break;

        case BuildingType::fowl_hunters:
            profile.outputs.push_back({ResourceType::wild_fowl, 10});
            break;

        case BuildingType::fishermen:
            profile.outputs.push_back({ResourceType::fish, 10});
            break;

        case BuildingType::beachcombers:
            profile.outputs.push_back({ResourceType::shellfish, 6});
            break;

        case BuildingType::gatherers:
            profile.outputs.push_back({ResourceType::wheat, 2});
            profile.outputs.push_back({ResourceType::barley, 2});
            profile.outputs.push_back({ResourceType::fodder, 2});
            profile.outputs.push_back({ResourceType::onion, 1});
            profile.outputs.push_back({ResourceType::turnip, 1});
            profile.outputs.push_back({ResourceType::apples, 1});
            profile.outputs.push_back({ResourceType::dill, 1});
            break;

        case BuildingType::loggers:
            profile.outputs.push_back({ResourceType::wood, 10});
            break;

        case BuildingType::flint_knappers:
            profile.outputs.push_back({ResourceType::flint, 10});
            break;

        case BuildingType::basalt_polishers:
            profile.outputs.push_back({ResourceType::basalt, 10});
            break;

        case BuildingType::game_butchers:
            profile.inputs.push_back({ResourceType::wild_game, 10});
            profile.outputs.push_back({ResourceType::meat, 8});
            profile.outputs.push_back({ResourceType::hide, 3});
            profile.outputs.push_back({ResourceType::bone, 2});
            profile.outputs.push_back({ResourceType::horn, 1});
            profile.outputs.push_back({ResourceType::lard, 2});
            break;

        case BuildingType::fowl_butchers:
            profile.inputs.push_back({ResourceType::wild_fowl, 10});
            profile.outputs.push_back({ResourceType::meat, 6});
            profile.outputs.push_back({ResourceType::bone, 1});
            profile.outputs.push_back({ResourceType::lard, 1});
            profile.outputs.push_back({ResourceType::feathers, 4});
            break;

        case BuildingType::feasting:
            profile.inputs.push_back({ResourceType::meat, 10});
            profile.outputs.push_back({ResourceType::prestige, 2});
            break;

        case BuildingType::charcoal_maker:
            profile.inputs.push_back({ResourceType::wood, 10});
            profile.outputs.push_back({ResourceType::charcoal, 5});
            break;

        case BuildingType::potter:
            profile.inputs.push_back({ResourceType::raw_clay, 8});
            profile.outputs.push_back({ResourceType::pottery, 4});
            break;

        case BuildingType::tanner:
            profile.inputs.push_back({ResourceType::hide, 6});
            profile.outputs.push_back({ResourceType::leather, 4});
            break;

        case BuildingType::thatcher:
            profile.outputs.push_back({ResourceType::thatch, 10});
            break;

        case BuildingType::ropemaker:
            profile.outputs.push_back({ResourceType::rope, 10});
            break;

        case BuildingType::deer_hunters:
            profile.outputs.push_back({ResourceType::deer, 10});
            break;

        case BuildingType::boar_hunters:
            profile.outputs.push_back({ResourceType::boar, 10});
            break;

        case BuildingType::hare_trappers:
            profile.outputs.push_back({ResourceType::hare, 8});
            break;

        case BuildingType::beaver_trappers:
            profile.outputs.push_back({ResourceType::beaver, 6});
            profile.outputs.push_back({ResourceType::hide, 2});
            break;

        case BuildingType::seal_hunters:
            profile.outputs.push_back({ResourceType::seal, 6});
            profile.outputs.push_back({ResourceType::hide, 3});
            profile.outputs.push_back({ResourceType::lard, 3});
            break;

        case BuildingType::whale_hunters:
            profile.outputs.push_back({ResourceType::whale, 2});
            profile.outputs.push_back({ResourceType::bone, 4});
            profile.outputs.push_back({ResourceType::lard, 6});
            break;

        case BuildingType::salmon_fishers:
            profile.outputs.push_back({ResourceType::salmon, 10});
            break;

        case BuildingType::river_fishers:
            profile.outputs.push_back({ResourceType::river_fish, 10});
            break;

        case BuildingType::lake_fishers:
            profile.outputs.push_back({ResourceType::lake_fish, 10});
            break;

        case BuildingType::eel_trappers:
            profile.outputs.push_back({ResourceType::eel, 8});
            break;

        case BuildingType::shellfish_gatherers:
            profile.outputs.push_back({ResourceType::shellfish, 10});
            break;

        case BuildingType::crab_gatherers:
            profile.outputs.push_back({ResourceType::crab, 8});
            break;

        case BuildingType::egg_gatherers:
            profile.outputs.push_back({ResourceType::eggs, 8});
            break;

        case BuildingType::honey_gatherers:
            profile.outputs.push_back({ResourceType::honey, 5});
            break;

        case BuildingType::berry_gatherers:
            profile.outputs.push_back({ResourceType::berries, 10});
            break;

        case BuildingType::nut_gatherers:
            profile.outputs.push_back({ResourceType::nuts, 8});
            break;

        case BuildingType::root_gatherers:
            profile.outputs.push_back({ResourceType::roots, 10});
            break;

        case BuildingType::mushroom_gatherers:
            profile.outputs.push_back({ResourceType::mushrooms, 7});
            break;

        case BuildingType::herb_gatherers:
            profile.outputs.push_back({ResourceType::herbs, 6});
            break;

        case BuildingType::reed_cutters:
            profile.outputs.push_back({ResourceType::reeds, 10});
            break;

        case BuildingType::fiber_gatherers:
            profile.outputs.push_back({ResourceType::plant_fiber, 10});
            break;

        case BuildingType::resin_tappers:
            profile.outputs.push_back({ResourceType::resin, 6});
            break;

        case BuildingType::bark_strippers:
            profile.outputs.push_back({ResourceType::bark, 8});
            break;

        case BuildingType::clay_diggers:
            profile.outputs.push_back({ResourceType::raw_clay, 10});
            break;

        case BuildingType::sand_gatherers:
            profile.outputs.push_back({ResourceType::sand, 10});
            break;

        case BuildingType::limestone_gatherers:
            profile.outputs.push_back({ResourceType::limestone, 8});
            break;

        case BuildingType::granite_quarriers:
            profile.outputs.push_back({ResourceType::granite, 6});
            break;

        case BuildingType::obsidian_knappers:
            profile.outputs.push_back({ResourceType::obsidian, 8});
            break;

        case BuildingType::copper_collectors:
            profile.outputs.push_back({ResourceType::native_copper, 4});
            break;

        case BuildingType::bog_iron_collectors:
            profile.outputs.push_back({ResourceType::bog_iron, 4});
            break;

        case BuildingType::salt_gatherers:
            profile.outputs.push_back({ResourceType::salt, 8});
            break;

        case BuildingType::peat_cutters:
            profile.outputs.push_back({ResourceType::peat, 10});
            break;

        case BuildingType::hide_workers:
            profile.inputs.push_back({ResourceType::hide, 6});
            profile.outputs.push_back({ResourceType::leather, 4});
            break;

        case BuildingType::bone_carvers:
            profile.inputs.push_back({ResourceType::bone, 5});
            profile.outputs.push_back({ResourceType::worked_bone, 4});
            break;

        case BuildingType::antler_workers:
            profile.inputs.push_back({ResourceType::antler, 5});
            profile.outputs.push_back({ResourceType::worked_antler, 4});
            break;

        case BuildingType::horn_workers:
            profile.inputs.push_back({ResourceType::horn, 5});
            profile.outputs.push_back({ResourceType::worked_horn, 4});
            break;

        case BuildingType::feather_workers:
            profile.inputs.push_back({ResourceType::feathers, 6});
            profile.outputs.push_back({ResourceType::prestige, 1});
            break;

        case BuildingType::basket_weavers:
            profile.inputs.push_back({ResourceType::reeds, 6});
            profile.outputs.push_back({ResourceType::baskets, 4});
            break;

        case BuildingType::mat_weavers:
            profile.inputs.push_back({ResourceType::reeds, 6});
            profile.outputs.push_back({ResourceType::mats, 4});
            break;

        case BuildingType::netmakers:
            profile.inputs.push_back({ResourceType::plant_fiber, 6});
            profile.outputs.push_back({ResourceType::nets, 4});
            break;

        case BuildingType::shell_workers:
            profile.inputs.push_back({ResourceType::shellfish, 4});
            profile.outputs.push_back({ResourceType::worked_shell, 3});
            break;

        case BuildingType::stone_polishers:
            profile.inputs.push_back({ResourceType::basalt, 5});
            profile.outputs.push_back({ResourceType::worked_stone, 4});
            break;

        case BuildingType::copper_workers:
            profile.inputs.push_back({ResourceType::native_copper, 4});
            profile.outputs.push_back({ResourceType::copper_tools, 2});
            break;

        case BuildingType::salt_curers:
            profile.inputs.push_back({ResourceType::meat, 8});
            profile.inputs.push_back({ResourceType::salt, 2});
            profile.outputs.push_back({ResourceType::cured_meat, 6});
            break;

        case BuildingType::smokehouses:
            profile.inputs.push_back({ResourceType::fish, 8});
            profile.inputs.push_back({ResourceType::wood, 2});
            profile.outputs.push_back({ResourceType::smoked_fish, 6});
            break;

        case BuildingType::drying_racks:
            profile.inputs.push_back({ResourceType::berries, 8});
            profile.outputs.push_back({ResourceType::dried_berries, 4});
            break;

        default:
            break;
    }

    return profile;
}