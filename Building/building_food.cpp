// File: building/building_food.cpp
// Commit: Split building food profile logic into dedicated source file

#include "building.h"

FoodProfile foodProfileForResource(ResourceType type)
{
    FoodProfile profile;

    switch (type)
    {
        case ResourceType::wild_game:
        case ResourceType::deer:
        case ResourceType::boar:
        case ResourceType::beaver:
            profile.fat = 7;
            profile.carbs = 0;
            profile.protein = 9;
            profile.luxury = 1;
            break;

        case ResourceType::hare:
            profile.fat = 2;
            profile.carbs = 0;
            profile.protein = 7;
            profile.luxury = 1;
            break;

        case ResourceType::wild_fowl:
            profile.fat = 4;
            profile.carbs = 0;
            profile.protein = 6;
            profile.luxury = 1;
            break;

        case ResourceType::meat:
            profile.fat = 8;
            profile.carbs = 0;
            profile.protein = 9;
            profile.luxury = 2;
            break;

        case ResourceType::seal:
        case ResourceType::whale:
            profile.fat = 10;
            profile.carbs = 0;
            profile.protein = 8;
            profile.luxury = 3;
            break;

        case ResourceType::fish:
        case ResourceType::salmon:
        case ResourceType::river_fish:
        case ResourceType::lake_fish:
        case ResourceType::eel:
            profile.fat = 3;
            profile.carbs = 0;
            profile.protein = 8;
            profile.luxury = 1;
            break;

        case ResourceType::shellfish:
        case ResourceType::crab:
            profile.fat = 1;
            profile.carbs = 0;
            profile.protein = 6;
            profile.luxury = 3;
            break;

        case ResourceType::wheat:
            profile.fat = 0;
            profile.carbs = 7;
            profile.protein = 2;
            profile.luxury = 0;
            break;

        case ResourceType::barley:
            profile.fat = 0;
            profile.carbs = 6;
            profile.protein = 2;
            profile.luxury = 0;
            break;

        case ResourceType::onion:
            profile.fat = 0;
            profile.carbs = 1;
            profile.protein = 0;
            profile.luxury = 2;
            break;

        case ResourceType::turnip:
        case ResourceType::roots:
            profile.fat = 0;
            profile.carbs = 3;
            profile.protein = 1;
            profile.luxury = 0;
            break;

        case ResourceType::apples:
        case ResourceType::berries:
            profile.fat = 0;
            profile.carbs = 3;
            profile.protein = 0;
            profile.luxury = 2;
            break;

        case ResourceType::dill:
        case ResourceType::herbs:
            profile.fat = 0;
            profile.carbs = 0;
            profile.protein = 0;
            profile.luxury = 3;
            break;

        case ResourceType::lard:
            profile.fat = 10;
            profile.carbs = 0;
            profile.protein = 0;
            profile.luxury = 1;
            break;

        case ResourceType::eggs:
            profile.fat = 5;
            profile.carbs = 0;
            profile.protein = 6;
            profile.luxury = 2;
            break;

        case ResourceType::honey:
            profile.fat = 0;
            profile.carbs = 8;
            profile.protein = 0;
            profile.luxury = 4;
            break;

        case ResourceType::nuts:
            profile.fat = 7;
            profile.carbs = 2;
            profile.protein = 4;
            profile.luxury = 2;
            break;

        case ResourceType::mushrooms:
            profile.fat = 0;
            profile.carbs = 1;
            profile.protein = 2;
            profile.luxury = 2;
            break;

        case ResourceType::cured_meat:
            profile.fat = 7;
            profile.carbs = 0;
            profile.protein = 8;
            profile.luxury = 3;
            break;

        case ResourceType::smoked_fish:
        case ResourceType::dried_fish:
            profile.fat = 3;
            profile.carbs = 0;
            profile.protein = 8;
            profile.luxury = 2;
            break;

        case ResourceType::dried_berries:
            profile.fat = 0;
            profile.carbs = 5;
            profile.protein = 0;
            profile.luxury = 3;
            break;

        default:
            break;
    }

    return profile;
}

bool isFoodResource(ResourceType type)
{
    const FoodProfile profile = foodProfileForResource(type);

    return
        profile.fat > 0 ||
        profile.carbs > 0 ||
        profile.protein > 0 ||
        profile.luxury > 0;
}