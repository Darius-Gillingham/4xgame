// File: game.h
// Commit: Add skeleton national market inventory to country state

#pragma once

#include "map.h"
#include "building/province.h"

#include <string>
#include <vector>

enum class Government
{
    tribal,
    aristocratic,
    plutocratic,
    theocratic
};

enum class RulerTrait
{
    martial,
    builder,
    pious,
    trader,
    scholar,
    administrator
};

struct PlayerSetup
{
    Region startRegion = Region::british_isles;
    Government government = Government::tribal;
    std::string rulerName = "Ruler";
    int rulerAge = 30;
    RulerTrait primaryTrait = RulerTrait::martial;
    RulerTrait secondaryTrait = RulerTrait::builder;
};

struct PlayerCounty
{
    int countryId = 0;
    int countyX = -1;
    int countyY = -1;
    int provinceId = -1;
    PlayerSetup setup;
};

struct GameDate
{
    int year = 1;
    int month = 1;
};

struct Discovery
{
    ResourceType resource = ResourceType::wild_game;
    bool discovered = false;
    int progress = 0;
    int requiredProgress = 1000;
};

struct NationalMarket
{
    std::vector<ResourceAmount> goods;
};

struct Country
{
    int id = -1;
    int capitalProvinceId = -1;
    int prestige = 10000;
    int piety = 10000;
    std::vector<int> provinceIds;
    std::vector<Discovery> discoveries;
    NationalMarket market;
};

struct ProvinceSlot
{
    int provinceId = -1;
    int countryId = -1;
    Province province;
};

struct Unit
{
    int id = -1;
    int countryId = -1;

    int sourceProvinceId = -1;
    int sourceHoldingId = -1;
    int sourceBuildingId = -1;
    int sourceLevySlotIndex = -1;

    MapPosition position;
    MapPosition destination;

    bool moving = false;
    int strength = 1;
};

struct GameState
{
    PlayerCounty playerCounty;
    std::vector<ProvinceSlot> provinceSlots;
    std::vector<Country> countries;
    std::vector<Unit> units;
    int nextUnitId = 1;
    int tickCount = 0;
    GameDate date;
};

const char* governmentName(Government government);
const char* rulerTraitName(RulerTrait trait);
const char* regionName(Region region);
const char* monthName(int month);

ProvinceSlot* provinceSlotById(
    GameState& gameState,
    int provinceId
);

const ProvinceSlot* provinceSlotById(
    const GameState& gameState,
    int provinceId
);

Province* provinceById(
    GameState& gameState,
    int provinceId
);

const Province* provinceById(
    const GameState& gameState,
    int provinceId
);

Country* countryById(
    GameState& gameState,
    int countryId
);

const Country* countryById(
    const GameState& gameState,
    int countryId
);

bool spawnPlayerCounty(
    Map& map,
    PlayerCounty& playerCounty
);

bool initializeGameState(
    Map& map,
    const PlayerSetup& setup,
    GameState& gameState
);

void synchronizeRaisedLevyUnits(
    const Map& map,
    GameState& gameState
);

void advanceGameMonth(
    GameState& gameState
);

void tickGame(
    GameState& gameState
);   